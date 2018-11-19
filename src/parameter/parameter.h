#pragma once

#include <string>
#include <vector>

#include "../utils/generic.h"
#include "../utils/types.h"


// TODO : Parameter are identified by the name field but it's also used for display
// Need a proper identifier and an optional display name

class Parameter
{
  public:
    enum class Type {
        Unknown,
        Text,
        Select,
        FilePath,
        Button,
        CheckBox,
        Slider,
        Matrix4x4,
        Curve,
    };

  public:
    Parameter() = default;
    Parameter(const std::string &name, Type t) : name(name), type(t) {}
    virtual Parameter& operator=(const Parameter &rhs) = default;
    virtual ~Parameter() = default;

    operator bool () const { return name != ""; }

  public:
    std::string name;
    Type type;
};


class TextParameter : public Parameter
{
  public:
    TextParameter() = default;
    TextParameter(const std::string &name) : Parameter(name, Type::Text) {}
    TextParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const TextParameter*>(&rhs);
        return *this;
    }

  public:
    std::string value;
    std::string default_value;
};

class SelectParameter : public Parameter
{
  public:
    SelectParameter() = default;
    SelectParameter(const std::string &name) : Parameter(name, Type::Select) {}
    SelectParameter(const std::string &name, const std::vector<std::string> &choices)
    : Parameter(name, Type::Select), choices(choices)
    {
        if (choices.size() > 0)
            value = default_value = choices[0];
    }
    SelectParameter(const std::string &name, std::vector<std::string> choices, const std::string &default_value)
    : Parameter(name, Type::Select), value(default_value), default_value(default_value), choices(choices)
    {

    }
    SelectParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const SelectParameter*>(&rhs);
        return *this;
    }

  public:
    std::string value;
    std::string default_value;
    std::vector<std::string> choices;
};

class FilePathParameter : public Parameter
{
  public:
    enum class PathType {
        File,
        Folder
    };

  public:
    FilePathParameter() = default;
    FilePathParameter(const std::string &name) : Parameter(name, Type::FilePath), path_type(PathType::File) {}
    FilePathParameter(const std::string &name, const std::string &value, const std::string &dialog_title = "", const std::string &filters = "", PathType pt = PathType::File)
    : Parameter(name, Type::FilePath), value(value), dialog_title(dialog_title), filters(filters), path_type(pt)
    {

    }
    FilePathParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const FilePathParameter*>(&rhs);
        return *this;
    }

  public:
    std::string value;
    std::string dialog_title;
    std::string filters;
    PathType path_type;
};

class ButtonParameter : public Parameter
{
  public:
    ButtonParameter() = default;
    ButtonParameter(const std::string &name) : Parameter(name, Type::Button) {}
    ButtonParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const ButtonParameter*>(&rhs);
        return *this;
    }
};

class CheckBoxParameter : public Parameter
{
  public:
    CheckBoxParameter() = default;
    CheckBoxParameter(const std::string &name, bool value) : Parameter(name, Type::CheckBox), value(value), default_value(value) {}
    CheckBoxParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const CheckBoxParameter*>(&rhs);
        return *this;
    }

  public:
    bool value;
    bool default_value;
};




class SliderParameter : public Parameter
{
  public:
    enum SliderScale {
        Linear,
        Log,
    };
    SliderParameter() = default;
    SliderParameter(const std::string &name, float value, float min, float max, float step, SliderScale scale) : Parameter(name, Type::Slider), value(value), default_value(value), min(min), max(max), step(step), scale(scale) {}
    SliderParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const SliderParameter*>(&rhs);
        return *this;
    }

  public:
    float value;
    float default_value;
    float min;
    float max;
    float step;
    SliderScale scale;
};

class Matrix4x4Parameter : public Parameter
{
  public:
    Matrix4x4Parameter() = default;
    Matrix4x4Parameter(const std::string &name) : Parameter(name, Type::Matrix4x4) {}
    Matrix4x4Parameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const Matrix4x4Parameter*>(&rhs);
        return *this;
    }

  public:
    float value[16];
    float default_value[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
};

class CurveParameter : public Parameter
{
  public:
    CurveParameter() = default;
    CurveParameter(const std::string &name) : Parameter(name, Type::Curve) {}
    CurveParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const CurveParameter*>(&rhs);
        return *this;
    }

  public:
    using ColorCallback = FuncT<ColorRGBA8(float x, float y)>;

    struct CurveTool {
        std::string name;

        CoordinateSystem coordinate_system;
        CurveInterpolation interpolation_method;

        std::string axis_legend[2];
        float axis_x_range[2];
        float axis_y_range[2];

        ColorRGBA8 background_color;
        ColorCallback background_color_cb;

        std::vector<Curve> curves;
    };

    std::vector<CurveTool> tools;
};