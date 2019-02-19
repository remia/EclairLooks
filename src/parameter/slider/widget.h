#pragma once

#include <gui/common/slider.h>
#include "../parameterwidget.h"


class ParameterSliderWidget : public ParameterWidget
{
  public:
    ParameterSliderWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    SliderParameter *m_sliderParam;
    SliderField *m_slider;
};
