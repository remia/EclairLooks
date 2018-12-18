#pragma once

#include <string>
#include <array>
#include "parameter.h"


class Matrix4x4Parameter : public Parameter
{
  public:
    using Matrix4x4 = std::array<float, 16>;

  public:
    Matrix4x4Parameter() = default;

    Matrix4x4Parameter(const std::string &name) : Parameter(name, Type::Matrix4x4) {}

  public:
    Matrix4x4 value() const { return m_value; }
    void setValue(const Matrix4x4 &v)
    {
        m_value = v;
        EmitEvent<UpdateValue>(*this);
    }

    Matrix4x4 defaultValue() const { return m_default_value; }
    void setDefaultValue(const Matrix4x4 &v)
    {
        m_default_value = v;
        EmitEvent<UpdateValue>(*this);
    }

  private:
    Matrix4x4 m_value;
    Matrix4x4 m_default_value = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 0.0f
    };
};