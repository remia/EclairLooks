#pragma once

#include <string>
#include "parameter.h"


class SelectParameter : public Parameter
{
  public:
    SelectParameter() = default;

    SelectParameter(const std::string &name) : Parameter(name, Type::Select) {}

    SelectParameter(const std::string &name, const std::vector<std::string> &choices)
        : Parameter(name, Type::Select), m_choices(choices)
    {
        if (m_choices.size() > 0)
            m_value = m_default_value = m_choices[0];
    }

    SelectParameter(const std::string &name, const std::vector<std::string> &choices,
                    const std::string &default_value)
        : Parameter(name, Type::Select), m_value(default_value),
          m_default_value(default_value), m_choices(choices)
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

    std::vector<std::string> choices() const { return m_choices; }
    void setChoices(const std::vector<std::string> &v)
    {
        m_choices = v;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    std::string m_value;
    std::string m_default_value;
    std::vector<std::string> m_choices;
};