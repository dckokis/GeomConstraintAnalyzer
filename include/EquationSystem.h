#pragma once

#include "Variable.h"
#include "Equation.h"

#include <vector>
#include <string>

class EquationSystem
{
public:
    void addVariable(Variable v);
    void addEquation(Equation e);

    [[nodiscard]] const std::vector<Variable>& variables() const { return m_variables; }
    [[nodiscard]] const std::vector<Equation>& equations() const { return m_equations; }

private:
    std::vector<Variable> m_variables;
    std::vector<Equation> m_equations;
};
