#pragma once

#include <string>
#include "parameter.h"


class TextParameter : public Parameter
{
  public:
    TextParameter() = default;

    TextParameter(const std::string &name) : Parameter(name, Type::Text)
    {

    }

  public:
    std::string value() const { return m_value; }
    void setValue(const std::string &v)
    {
        m_value = v;
        EmitEvent<UpdateValue>(*this);
    }

    std::string defaultValue() const { return m_default_value; }
    void setDefaultValue(const std::string &v)
    {
        m_default_value = v;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    std::string m_value;
    std::string m_default_value;
};