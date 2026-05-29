#pragma once

#include <array>
#include <string>

enum class VarType
{
    Point,
    Vector,
    Scalar
};

class Variable
{
public:
    Variable(int id, VarType type, double v1, double v2, bool fixed);
    Variable(int id, VarType type, double scalar, bool fixed);

    [[nodiscard]] int id() const { return m_id; }
    [[nodiscard]] VarType type() const { return m_type; }
    [[nodiscard]] bool isFixed() const { return m_fixed; }

    [[nodiscard]] double x() const;
    [[nodiscard]] double y() const;
    [[nodiscard]] double value() const;

    void setValue(double v1, double v2 = 0.0);

private:
    int m_id;
    VarType m_type;
    bool m_fixed;
    std::array<double, 2> m_value;
};
