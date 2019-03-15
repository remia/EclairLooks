#pragma once

#include <parameter/parameterwidget.h>
#include "curve.h"


class CurveScene;
class CurveView;
class CurveEditParameter;
class CurveParameter;
class QButtonGroup;

class ParameterCurveEditWidget : public ParameterWidget
{
  public:
    ParameterCurveEditWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void updateWidget(const Parameter &p) override;

  private:
    void initWidget();

    void addCurve(CurveParameter *curve);
    void selectCurve(int index);
    void mixCurve(int index, float v);
    void resetCurve(int index);

  private:
    CurveEditParameter *m_curveEditParam;

    CurveScene *m_scene;
    CurveView *m_view;

    QHBoxLayout *m_selectLayout;
    QVBoxLayout *m_curveLayout;
    QButtonGroup *m_selectBtns;
    QButtonGroup *m_resetBtns;
};
