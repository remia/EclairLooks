#pragma once

#include <string>
#include <vector>
#include <any>


struct ImageOperatorParameter
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

    Type type;
    std::string name;

    std::any value;
    std::any values;
    std::any default_val;

    operator bool () const { return name != ""; }
};

using ImageOperatorParameterVec = std::vector<ImageOperatorParameter>;