#pragma once

#include "image.h"
#include "utils/generic.h"

using TransformFuncT = FuncT<void(Image&)>;

template <typename F>
void IsolateColorContrast(Image& img, F f, float color, float contrast);

#include "transform.hpp"