#pragma once

#include "../parameterwidget.h"


class ParameterMatrixWidget : public ParameterWidget
{
  public:
    ParameterMatrixWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void updateWidget(const Parameter &p) override;

  private:
    // MatrixParameter *m_matrixParam;
};
