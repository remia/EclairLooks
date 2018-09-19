#pragma once

#include "../utils/generic.h"

#include <QtWidgets/QWidget>


class LookWidget;
class ImageWidget;
class CurveWidget;
class QOpenGLWidget;

class LookDetailWidget : public QWidget
{
  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lw);

    void resetView(uint8_t id);
    void showDetail(const QString &path, uint8_t id);

  private:
    LookWidget *m_lookWidget;
    ImageWidget *m_imageWidget;
    CurveWidget *m_curveWidget;
    QOpenGLWidget *m_cubeWidget;
};