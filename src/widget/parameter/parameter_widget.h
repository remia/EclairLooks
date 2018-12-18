#pragma once

#include <QtWidgets/QtWidgets>

#include "../../utils/generic.h"
#include "../../parameter/parameter.h"

class QVBoxLayout;

class ParameterWidget : public QWidget
{
  public:
    ParameterWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateUi(const Parameter &p);
    virtual void UpdateWidget(const Parameter &p) {}

  protected:
    Parameter *m_param;
    QVBoxLayout *m_layout;
};

ParameterWidget* WidgetFromParameter(Parameter *p);

#include "parameter_checkbox.h"
#include "parameter_filepath.h"
#include "parameter_select.h"
#include "parameter_slider.h"
#include "parameter_text.h"