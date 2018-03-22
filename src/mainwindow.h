#pragma once

#include <QtWidgets/QMainWindow>


class ImageWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

  private:
    ImageWidget * m_imageWidget;
};