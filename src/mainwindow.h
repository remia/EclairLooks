#pragma once

#include <QtWidgets/QMainWindow>

#include "imagepipeline.h"


class ImageWidget;
class LogWidget;
class TransformationListWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

    ImagePipeline &pipeline() { return m_pipeline; }
    ImageWidget *viewer() { return m_imageWidget; }

  public:
    void keyPressEvent(QKeyEvent *event) override;

  private:
    ImageWidget * m_imageWidget;
    LogWidget * m_logWidget;
    TransformationListWidget * m_transformationsWidget;

    ImagePipeline m_pipeline;
};