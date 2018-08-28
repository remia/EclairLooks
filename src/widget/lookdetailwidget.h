#pragma once

#include "../utils/generic.h"

#include <QtWidgets/QWidget>


class ImagePipeline;
class ImageOperatorList;
class ImageWidget;
class QOpenGLWidget;

class LookDetailWidget : public QWidget
{
  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void setPipeline(ImagePipeline *pipeline);
    void setOperators(ImageOperatorList *list);

    void showDetail(const QString &path);

  private:
    ImagePipeline *m_globalPipeline;
    UPtr<ImagePipeline> m_pipeline;
    ImageOperatorList *m_operators;

    ImageWidget *m_imageWidget;
    QOpenGLWidget *m_curveWidget;
    QOpenGLWidget *m_cubeWidget;
};