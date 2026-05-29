#include "Variable.h"

#include <stdexcept>

Variable::Variable(int id, VarType type, double v1, double v2, bool fixed)
    : m_id(id), m_type(type), m_fixed(fixed), m_value{v1, v2}
{
    if (type == VarType::Scalar)
    {
        throw std::invalid_argument("Cannot construct Scalar variable with two coordinates. Use the single-value constructor.");
    }
}

Variable::Variable(int id, VarType type, double v1, bool fixed)
    : m_id(id), m_type(type), m_fixed(fixed), m_value{v1, 0.0}
{
    if (type != VarType::Scalar)
    {
        throw std::invalid_argument("Single-value constructor is only allowed for Scalar type.");
    }
}

double Variable::x() const
{
    if (m_type == VarType::Scalar)
    {
        throw std::logic_error("Scalar has no x coordinate");
    }
    return m_value[0];
}

double Variable::y() const
{
    if (m_type == VarType::Scalar)
    {
        throw std::logic_error("Scalar has no y coordinate");
    }
    return m_value[1];
}

double Variable::value() const
{
    if (m_type != VarType::Scalar)
    {
        throw std::logic_error("Point/Vector has no single value");
    }
    return m_value[0];
}

void Variable::setValue(double v1, double v2)
{
    m_value[0] = v1;
    m_value[1] = v2;
}
