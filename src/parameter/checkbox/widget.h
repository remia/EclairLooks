#pragma once

#include "../parameterwidget.h"


class ParameterCheckBoxWidget : public ParameterWidget
{
  public:
    ParameterCheckBoxWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    CheckBoxParameter *m_checkBoxParam;
    QCheckBox *m_checkBox;
};
