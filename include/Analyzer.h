#pragma once

#include "EquationSystem.h"

#include <vector>
#include <string>

enum class Diagnosis
{
    UnderDefined,
    WellDefined,
    OverDefined,
    Unknown
};

class Analyzer
{
public:
    explicit Analyzer(const EquationSystem& system);

    Diagnosis diagnose();

private:
    void buildJacobian();
    [[nodiscard]] int computeRankQR();

    const EquationSystem& m_system;
    int m_cols; // unfixed variables components
    int m_rows; // equations count
    std::vector<double> m_jacobian; // M x N row-major
    std::vector<int> m_varColumnStart; // full variable index -> column start (-1 if fixed)
};
