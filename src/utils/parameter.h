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
    virtual Parameter& operator=(const Parameter &rhs) = default;
    virtual ~Parameter() = default;

    std::string name;
    Type type;

    operator bool () const { return name != ""; }
};


struct TextParameter : public Parameter
{
    TextParameter() = default;
    TextParameter(const std::string &name) : Parameter(name, Type::Text) {}
    TextParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const TextParameter*>(&rhs);
        return *this;
    }

    std::string value;
    std::string default_value;
};

struct SelectParameter : public Parameter
{
    SelectParameter() = default;
    SelectParameter(const std::string &name) : Parameter(name, Type::Select) {}
    SelectParameter(const std::string &name, const std::vector<std::string> &choices) : Parameter(name, Type::Select), choices(choices) { if (choices.size() > 0) value = default_value = choices[0]; }
    SelectParameter(const std::string &name, std::vector<std::string> choices, const std::string &default_value) : Parameter(name, Type::Select), default_value(default_value), choices(choices) {}

    SelectParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const SelectParameter*>(&rhs);
        return *this;
    }

    std::string value;
    std::string default_value;
    std::vector<std::string> choices;
};

struct FilePathParameter : public Parameter
{
    FilePathParameter() = default;
    FilePathParameter(const std::string &name) : Parameter(name, Type::FilePath) {}
    FilePathParameter(const std::string &name, const std::string &value, const std::string &dialog_title = "", const std::string &filters = "") : Parameter(name, Type::FilePath), value(value), filters(filters) {}

    FilePathParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const FilePathParameter*>(&rhs);
        return *this;
    }

    std::string value;
    std::string default_value;
    std::string dialog_title;
    std::string filters;
};

struct ButtonParameter : public Parameter
{
    ButtonParameter() = default;
    ButtonParameter(const std::string &name) : Parameter(name, Type::Button) {}
    ButtonParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const ButtonParameter*>(&rhs);
        return *this;
    }
};

struct CheckBoxParameter : public Parameter
{
    CheckBoxParameter() = default;
    CheckBoxParameter(const std::string &name, bool value) : Parameter(name, Type::CheckBox), value(value), default_value(value) {}
    CheckBoxParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const CheckBoxParameter*>(&rhs);
        return *this;
    }

    bool value;
    bool default_value;
};

struct SliderParameter : public Parameter
{
    SliderParameter() = default;
    SliderParameter(const std::string &name, float value, float min, float max, float step) : Parameter(name, Type::Slider), value(value), default_value(value), min(min), max(max), step(step) {}
    SliderParameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const SliderParameter*>(&rhs);
        return *this;
    }

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
    Matrix4x4Parameter& operator=(const Parameter &rhs) override
    {
        *this = *static_cast<const Matrix4x4Parameter*>(&rhs);
        return *this;
    }

    float value[16];
    float default_value[16] = {
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    };
};
