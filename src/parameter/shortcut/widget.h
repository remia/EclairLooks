#pragma once

#include "../parameterwidget.h"


class ParameterShortcutWidget : public ParameterWidget
{
  public:
    ParameterShortcutWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void updateWidget(const Parameter &p) override;

  private:
    ShortcutParameter *m_shortcutParam;
    QKeySequenceEdit *m_keyEdit;
};