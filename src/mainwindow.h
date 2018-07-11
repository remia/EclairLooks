#pragma once

#include <QtWidgets/QMainWindow>

#include "imagepipeline.h"

class QToolBar;

class ImageWidget;
class LogWidget;
class TransformationListWidget;
class WaveformWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

    ImagePipeline &pipeline() { return m_pipeline; }
    ImageWidget *viewer() { return m_imageWidget; }

  public:
    void keyPressEvent(QKeyEvent *event) override;

  private:
    QToolBar *m_toolBar;
    LogWidget *m_logWidget;
    ImageWidget *m_imageWidget;
    TransformationListWidget *m_transformationsWidget;
    WaveformWidget *m_waveformWidget;

    ImagePipeline m_pipeline;
};