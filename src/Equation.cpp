#include "Equation.h"

#include <stdexcept>

std::size_t Equation::expectedVarCount(EquationType t)
{
    switch (t)
    {
    case EquationType::Norm:
        return 2;
    case EquationType::Cross:
    case EquationType::Dot:
    case EquationType::Distance:
        return 3;
    case EquationType::Orientation:
    case EquationType::Projection:
        return 4;
    }
    return 0;
}

Equation::Equation(EquationType type, std::vector<int> varIds)
    : m_type(type), m_varIds(std::move(varIds))
{
    if (m_varIds.size() != expectedVarCount(type))
    {
        throw std::invalid_argument("Equation: wrong number of variable ids");
    }
}
