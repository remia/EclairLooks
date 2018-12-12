#pragma once

#include <string>
#include "parameter.h"


class CheckBoxParameter : public Parameter
{
  public:
    CheckBoxParameter() = default;

    CheckBoxParameter(const std::string &name, bool value)
        : Parameter(name, Type::CheckBox), m_value(value), m_default_value(value)
    {
    }

  public:
    bool value() const { return m_value; }
    void setValue(const bool &v)
    {
        m_value = v;
        EmitEvent<UpdateValue>(*this);
    }

    bool defaultValue() const { return m_default_value; }
    void setDefaultValue(const bool &v)
    {
        m_default_value = v;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    bool m_value;
    bool m_default_value;
};