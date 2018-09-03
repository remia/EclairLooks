#pragma once

#include "../utils/generic.h"

#include <QtWidgets/QWidget>


class LookWidget;
class ImageWidget;
class QOpenGLWidget;

class LookDetailWidget : public QWidget
{
  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lw);

    void showDetail(const QString &path);

  private:
    LookWidget *m_lookWidget;
    ImageWidget *m_imageWidget;
    QOpenGLWidget *m_curveWidget;
    QOpenGLWidget *m_cubeWidget;
};