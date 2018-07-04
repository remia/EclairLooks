#include "mainwindow.h"
#include "widget/imagewidget.h"
#include "widget/logwidget.h"
#include "widget/transformationlistwidget.h"

#include <QtWidgets/QDockWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_logWidget = new LogWidget();
    m_imageWidget = new ImageWidget();
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

    using std::placeholders::_1;
    m_pipeline.RegisterResetCallback(std::bind(&ImageWidget::setImage, m_imageWidget, _1));
    m_pipeline.RegisterUpdateCallback(std::bind(&ImageWidget::updateImage, m_imageWidget, _1));
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