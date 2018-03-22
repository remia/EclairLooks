#include "mainwindow.h"
#include "imagewidget.h"
#include "logwidget.h"

#include <QtCore/QMimeData>
#include <QtWidgets/QDockWidget>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    m_imageWidget = new ImageWidget(this);

    setAcceptDrops(true);
    setWindowTitle("Eclair Look");
    setCentralWidget(m_imageWidget);

    QDockWidget *logDockWidget = new QDockWidget("Log");
    logDockWidget->setWidget(new LogWidget());
    addDockWidget(Qt::BottomDockWidgetArea, logDockWidget);
}

void MainWindow::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainWindow::dropEvent(QDropEvent *e)
{
    foreach (const QUrl &url, e->mimeData()->urls()) {
        QString fileName = url.toLocalFile();
        qDebug() << "Dropped file:" << fileName << "\n";

        m_imageWidget->initializeTexture(fileName.toStdString());
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Escape:
        close();
        break;
      case Qt::Key_Backspace:
        m_imageWidget->clearTexture();
        break;
      default:
        QMainWindow::keyPressEvent(event);
  }
}