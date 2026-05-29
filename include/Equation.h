#pragma once

#include <vector>

enum class EquationType
{
    Norm, // v^2 - r = 0
    Cross, // v1 x v2 - s = 0
    Dot, // v1 * v2 - c = 0
    Distance, // (p1-p2)^2 - r2 = 0
    Orientation, // (p1-p2) x v - s = 0
    Projection // (p1-p2) * v - c = 0
};

class Equation
{
public:
    Equation(EquationType type, std::vector<int> varIds);

    [[nodiscard]] EquationType type() const { return m_type; }
    [[nodiscard]] const std::vector<int>& varIds() const { return m_varIds; }

private:
    // Number of variable references expected for each type
    static std::size_t expectedVarCount(EquationType t);

    EquationType m_type;
    std::vector<int> m_varIds; // indices into the system's variable list
};
