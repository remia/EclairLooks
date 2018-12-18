#pragma once

#include <string>
#include "parameter.h"


class SliderParameter : public Parameter
{
  public:
    enum Scale {
        Linear,
        Log,
    };

  public:
    SliderParameter() = default;

    SliderParameter(const std::string &name, float value, float min, float max,
                    float step, Scale scale = Scale::Linear)
        : Parameter(name, Type::Slider), m_value(value), m_default_value(value),
          m_min(min), m_max(max), m_step(step), m_scale(scale)
    {
    }

  public:
    float value() const { return m_value; }
    void setValue(const float &v)
    {
        m_value = v;
        EmitEvent<UpdateValue>(*this);
    }

    float defaultValue() const { return m_default_value; }
    void setDefaultValue(const float &v)
    {
        m_default_value = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    float min() const { return m_min; }
    void setMin(const float &v)
    {
        m_min = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    float max() const { return m_max; }
    void setMax(const float &v)
    {
        m_max = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    float step() const { return m_step; }
    void setStep(const float &v)
    {
        m_step = v;
        EmitEvent<UpdateSpecification>(*this);
    }

    Scale scale() const { return m_scale; }
    void setScale(const Scale &v)
    {
        m_scale = v;
        EmitEvent<UpdateSpecification>(*this);
    }

  private:
    float m_value;
    float m_default_value;
    float m_min;
    float m_max;
    float m_step;
    Scale m_scale;
};