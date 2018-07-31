#pragma once

#include <QtWidgets/QMainWindow>


class ImagePipeline;
class DevWidget;
class LogWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(ImagePipeline *pipeline, QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;

  public:
    ImagePipeline *pipeline();

  private:
    ImagePipeline *m_pipeline;

    QMenu *m_fileMenu;

    QTabWidget *m_tabWidget;
    LogWidget *m_logWidget;
    DevWidget *m_devWidget;
};