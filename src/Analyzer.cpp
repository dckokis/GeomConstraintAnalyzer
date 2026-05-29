#include "Analyzer.h"

#include <cmath>
#include <algorithm>
#include <numeric>

Analyzer::Analyzer(const EquationSystem& system)
    : m_system(system), m_cols(0), m_rows(static_cast<int>(system.equations().size()))
{
    // Map variables to column start indices
    m_varColumnStart.resize(system.variables().size(), -1);
    int col = 0;
    for (size_t i = 0; i < system.variables().size(); ++i)
    {
        const Variable& v = system.variables()[i];
        if (!v.isFixed())
        {
            m_varColumnStart[i] = col;
            col += v.type() == VarType::Scalar ? 1 : 2;
        }
    }
    m_cols = col;
}

void Analyzer::buildJacobian()
{
    m_jacobian.assign(m_rows * m_cols, 0.0);

    const auto& vars = m_system.variables();
    const auto& eqs = m_system.equations();

    for (int i = 0; i < m_rows; ++i)
    {
        const Equation& eq = eqs[i];
        const auto& ids = eq.varIds();

        // Helper to set derivative into matrix
        auto setDeriv = [&](int varIdx, int component, double val)
        {
            int colStart = m_varColumnStart[varIdx];
            if (colStart == -1) // fixed var
                return;
            int col = colStart + component;
            m_jacobian[i * m_cols + col] += val; // accumulate contributions
        };

        switch (eq.type())
        {
        case EquationType::Norm:
            {
                const Variable& v = vars[ids[0]];
                double vx = v.x(), vy = v.y();
                setDeriv(ids[0], 0, 2 * vx);
                setDeriv(ids[0], 1, 2 * vy);
                setDeriv(ids[1], 0, -1.0);
                break;
            }
        case EquationType::Cross:
            {
                const Variable& v1 = vars[ids[0]];
                const Variable& v2 = vars[ids[1]];
                double v1x = v1.x(), v1y = v1.y();
                double v2x = v2.x(), v2y = v2.y();
                setDeriv(ids[0], 0, v2y);
                setDeriv(ids[0], 1, -v2x);
                setDeriv(ids[1], 0, -v1y);
                setDeriv(ids[1], 1, v1x);
                setDeriv(ids[2], 0, -1.0);
                break;
            }
        case EquationType::Dot:
            {
                const Variable& v1 = vars[ids[0]];
                const Variable& v2 = vars[ids[1]];
                setDeriv(ids[0], 0, v2.x());
                setDeriv(ids[0], 1, v2.y());
                setDeriv(ids[1], 0, v1.x());
                setDeriv(ids[1], 1, v1.y());
                setDeriv(ids[2], 0, -1.0);
                break;
            }
        case EquationType::Distance:
            {
                const Variable& p1 = vars[ids[0]];
                const Variable& p2 = vars[ids[1]];
                double dx = p1.x() - p2.x();
                double dy = p1.y() - p2.y();
                setDeriv(ids[0], 0, 2 * dx);
                setDeriv(ids[0], 1, 2 * dy);
                setDeriv(ids[1], 0, -2 * dx);
                setDeriv(ids[1], 1, -2 * dy);
                setDeriv(ids[2], 0, -1.0);
                break;
            }
        case EquationType::Orientation:
            {
                const Variable& p1 = vars[ids[0]];
                const Variable& p2 = vars[ids[1]];
                const Variable& v = vars[ids[2]];
                double dx = p1.x() - p2.x();
                double dy = p1.y() - p2.y();
                setDeriv(ids[0], 0, v.y());
                setDeriv(ids[0], 1, -v.x());
                setDeriv(ids[1], 0, -v.y());
                setDeriv(ids[1], 1, v.x());
                setDeriv(ids[2], 0, -dy);
                setDeriv(ids[2], 1, dx);
                setDeriv(ids[3], 0, -1.0);
                break;
            }
        case EquationType::Projection:
            {
                const Variable& p1 = vars[ids[0]];
                const Variable& p2 = vars[ids[1]];
                const Variable& v = vars[ids[2]];
                double dx = p1.x() - p2.x();
                double dy = p1.y() - p2.y();
                setDeriv(ids[0], 0, v.x());
                setDeriv(ids[0], 1, v.y());
                setDeriv(ids[1], 0, -v.x());
                setDeriv(ids[1], 1, -v.y());
                setDeriv(ids[2], 0, dx);
                setDeriv(ids[2], 1, dy);
                setDeriv(ids[3], 0, -1.0);
                break;
            }
        }
    }
}

int Analyzer::computeRankQR()
{
    if (m_rows == 0 || m_cols == 0) return 0;

    constexpr double tol = 1e-9;
    std::vector<int> pivot(m_cols);
    std::iota(pivot.begin(), pivot.end(), 0);
    std::vector<double> tau(std::min(m_rows, m_cols));

    for (int col = 0, k = 0; col < m_cols && k < m_rows; ++col)
    {
        // Choose leading column
        double maxNorm = 0.0;
        int pivotCol = col;
        for (int j = col; j < m_cols; ++j)
        {
            double norm = 0.0;
            for (int i = k; i < m_rows; ++i)
            {
                double val = m_jacobian[i * m_cols + pivot[j]];
                norm += val * val;
            }
            if (norm > maxNorm)
            {
                maxNorm = norm;
                pivotCol = j;
            }
        }
        if (maxNorm < tol * tol) break;
        if (pivotCol != col)
        {
            std::swap(pivot[col], pivot[pivotCol]);
            for (int i = 0; i < m_rows; ++i)
            {
                std::swap(m_jacobian[i * m_cols + col], m_jacobian[i * m_cols + pivotCol]);
            }
        }

        // Householder reflection
        double nrm = 0.0;
        for (int i = k; i < m_rows; ++i)
        {
            nrm += m_jacobian[i * m_cols + col] * m_jacobian[i * m_cols + col];
        }
        nrm = std::sqrt(nrm);

        double x0 = m_jacobian[k * m_cols + col];
        double alpha = (x0 > 0) ? -nrm : nrm;
        double v0 = x0 - alpha;

        tau[k] = -v0 / alpha;

        m_jacobian[k * m_cols + col] = alpha;
        for (int i = k + 1; i < m_rows; ++i)
        {
            m_jacobian[i * m_cols + col] /= v0;
        }

        // Apply reflection to remaining columns
        for (int j = col + 1; j < m_cols; ++j)
        {
            double dot = m_jacobian[k * m_cols + j];
            for (int i = k + 1; i < m_rows; ++i)
            {
                dot += m_jacobian[i * m_cols + col] * m_jacobian[i * m_cols + j];
            }
            dot *= tau[k];
            m_jacobian[k * m_cols + j] -= dot;
            for (int i = k + 1; i < m_rows; ++i)
            {
                m_jacobian[i * m_cols + j] -= dot * m_jacobian[i * m_cols + col];
            }
        }
        ++k;
    }

    // Define rank by diagonal elements
    int rank = 0;
    double maxDiag = 0.0;
    for (int i = 0; i < std::min(m_rows, m_cols); ++i)
    {
        double d = std::abs(m_jacobian[i * m_cols + i]);
        if (d > maxDiag)
        {
            maxDiag = d;
        }
    }
    double threshold = tol * maxDiag;
    for (int i = 0; i < std::min(m_rows, m_cols); ++i)
    {
        if (std::abs(m_jacobian[i * m_cols + i]) > threshold)
        {
            ++rank;
            continue;
        }
        break;
    }
    return rank;
}

Diagnosis Analyzer::diagnose()
{
    buildJacobian();
    int rank = computeRankQR();
    int DOF = m_cols - rank;

    if (rank == m_rows && DOF == 0) { return Diagnosis::WellDefined; }
    if (rank == m_rows && DOF > 0) { return Diagnosis::UnderDefined; }
    if (rank < m_rows && DOF == 0) { return Diagnosis::OverDefined; }
    return Diagnosis::Unknown;
}
