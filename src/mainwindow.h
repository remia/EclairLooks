#pragma once

#include <QtWidgets/QMainWindow>

#include "imagepipeline.h"


class DevWidget;
class LogWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;

  public:
    ImagePipeline &pipeline() { return m_pipeline; }

  private:
    QTabWidget *m_tabWidget;

    LogWidget *m_logWidget;
    DevWidget *m_devWidget;

    ImagePipeline m_pipeline;
};