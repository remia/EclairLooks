#pragma once

#include <string>
#include <vector>
#include "parameter.h"
#include "../utils/types.h"


class CurveParameter : public Parameter
{
  public:
    CurveParameter() = default;

    CurveParameter(const std::string &name) : Parameter(name, Type::Curve) {}

  public:

  private:
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
