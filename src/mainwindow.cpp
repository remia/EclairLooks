#include "mainwindow.h"
#include "imagewidget.h"
#include "logwidget.h"
#include "transformationlistwidget.h"

#include <QtWidgets/QDockWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_imageWidget = new ImageWidget();
    m_logWidget = new LogWidget();
    m_transformationsWidget = new TransformationListWidget();

    setWindowTitle("Eclair Look");
    setCentralWidget(m_imageWidget);

    QDockWidget *dw = nullptr;
    dw = new QDockWidget("Log");
    dw->setWidget(m_logWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dw);

    dw = new QDockWidget("Transformations");
    dw->setWidget(m_transformationsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dw);
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Escape:
        close();
        break;
      case Qt::Key_Backspace:
        m_imageWidget->clearImage();
        break;
      default:
        QMainWindow::keyPressEvent(event);
  }
}