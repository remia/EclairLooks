#pragma once

#include <core/types.h>


struct Knots {
    float x;
    float y;
    float tan_angle;
    float tan_norm_left;
    float tan_norm_right;
};

struct Curve {
  float eval(float x);

  std::vector<Knots> knots;
};

using KnotsV = std::vector<Knots>;

class CurveInterpolator
{
  public:
    static std::vector<float> CatmullRom(const KnotsV &p, int size);
};