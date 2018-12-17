#pragma once

#include <string>
#include "parameter.h"

#include <QtCore/QDebug>


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
    std::vector<std::string> tooltips() const { return m_tooltips; }
    void setChoices(const std::vector<std::string> &v, const std::vector<std::string> &t = std::vector<std::string>())
    {
        if (t.size() > 0 && v.size() != t.size()) {
            qWarning() << "Choices / Tooltip size don't match !";
            return;
        }

        m_choices = v;
        m_tooltips = t;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    std::string m_value;
    std::string m_default_value;
    std::vector<std::string> m_choices;
    std::vector<std::string> m_tooltips;
};