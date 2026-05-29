#include "EquationSystem.h"

void EquationSystem::addVariable(Variable v)
{
    m_variables.push_back(v);
}

void EquationSystem::addEquation(Equation e)
{
    m_equations.push_back(std::move(e));
}
