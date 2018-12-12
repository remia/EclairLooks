#pragma once

#include <string>
#include <vector>

#include "../utils/generic.h"
#include "../utils/event_source.h"


class Parameter ;

typedef EventDesc<
    FuncT<void(const Parameter &p)>,
    FuncT<void(const Parameter &p)>> PEvtDesc;

class Parameter : public EventSource<PEvtDesc>
{
  public:
    enum Evt { UpdateValue, UpdateSpecification };

    enum class Type {
        Unknown,
        Button,
        CheckBox,
        Curve,
        FilePath,
        Matrix4x4,
        Select,
        Slider,
        Text,
    };

  public:
    Parameter() = default;

    Parameter(const std::string &name, Type t)
        : m_name(name), m_display_name(name), m_type(t)
    {
    }

    virtual ~Parameter() = default;

  public:
    operator bool () const { return m_name != ""; }

  public:
    std::string name() const { return m_name; }

    std::string displayName() const { return m_display_name; }
    void setDisplayName(const std::string &v) { m_display_name = v; }

    Type type() const { return m_type; }

  private:
    std::string m_name;
    std::string m_display_name;
    Type m_type;
};

#include "parameter_button.h"
#include "parameter_checkbox.h"
#include "parameter_curve.h"
#include "parameter_filepath.h"
#include "parameter_matrix.h"
#include "parameter_select.h"
#include "parameter_slider.h"
#include "parameter_text.h"