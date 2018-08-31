#pragma once

#include <string>
#include <vector>


struct Parameter
{
    enum class Type {
        Text,
        Select,
        FilePath,
        Button,
        CheckBox,
        Slider,
        Matrix4x4,
    };

    Parameter() = default;
    Parameter(const std::string &name, Type t) : name(name), type(t) {}

    std::string name;
    Type type;

    operator bool () const { return name != ""; }
};


struct TextParameter : public Parameter
{
    TextParameter() = default;
    TextParameter(const std::string &name) : Parameter(name, Type::Text) {}

    std::string value;
    std::string default_value;
};

struct SelectParameter : public Parameter
{
    SelectParameter() = default;
    SelectParameter(const std::string &name) : Parameter(name, Type::Select) {}
    SelectParameter(const std::string &name, const std::vector<std::string> &choices) : Parameter(name, Type::Select), choices(choices) { if (choices.size() > 0) value = default_value = choices[0]; }
    SelectParameter(const std::string &name, std::vector<std::string> choices, const std::string &default_value) : Parameter(name, Type::Select), default_value(default_value), choices(choices) {}

    std::string value;
    std::string default_value;
    std::vector<std::string> choices;
};

struct FilePathParameter : public Parameter
{
    FilePathParameter() = default;
    FilePathParameter(const std::string &name) : Parameter(name, Type::FilePath) {}
    FilePathParameter(const std::string &name, const std::string &value, const std::string &dialog_title = "", const std::string &filters = "") : Parameter(name, Type::FilePath), value(value), filters(filters) {}

    std::string value;
    std::string default_value;
    std::string dialog_title;
    std::string filters;
};

struct ButtonParameter : public Parameter
{
    ButtonParameter() = default;
    ButtonParameter(const std::string &name) : Parameter(name, Type::Button) {}
};

struct CheckBoxParameter : public Parameter
{
    CheckBoxParameter() = default;
    CheckBoxParameter(const std::string &name, bool value) : Parameter(name, Type::CheckBox), value(value), default_value(value) {}

    bool value;
    bool default_value;
};

struct SliderParameter : public Parameter
{
    SliderParameter() = default;
    SliderParameter(const std::string &name, float value, float min, float max, float step) : Parameter(name, Type::Slider), value(value), default_value(value), min(min), max(max), step(step) {}

    float value;
    float default_value;
    float min;
    float max;
    float step;
};

struct Matrix4x4Parameter : public Parameter
{
    Matrix4x4Parameter() = default;
    Matrix4x4Parameter(const std::string &name) : Parameter(name, Type::Matrix4x4) {}

    float value[16];
    float default_value[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
};
