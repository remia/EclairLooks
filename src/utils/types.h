#pragma once

#include <vector>


// ----------------------------------------------------------------------------

template <typename T>
struct Point2D
{
    T x;
    T y;
};

using Point2F = Point2D<float>;


template <typename T>
struct Color
{
    T r;
    T g;
    T b;
    T a;
};

using ColorRGBA8 = Color<uint8_t>;

// ----------------------------------------------------------------------------

struct ControlPoint {
    float x;
    float y;
    float tan_angle;
    float tan_norm;
};

struct Curve {
    std::string name;
    std::string short_name;
    float opacity;
    ColorRGBA8 color;

    std::vector<ControlPoint> points;
};

// ----------------------------------------------------------------------------

enum class CoordinateSystem { Cartesian, Polar };
enum class CurveInterpolation { Linear, Cubic, Bezier, Catmull_Rom };