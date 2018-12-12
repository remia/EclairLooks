#pragma once

#include <string>
#include "parameter.h"


class ButtonParameter : public Parameter
{
  public:
    ButtonParameter() = default;

    ButtonParameter(const std::string &name) : Parameter(name, Type::Button) {}
};
