#include "mainwindow.h"
#include "imagepipeline.h"
#include "widget/devwidget.h"
#include "widget/logwidget.h"

#include <QtWidgets/QtWidgets>


MainWindow::MainWindow(ImagePipeline *pipeline, QWidget *parent)
    : QMainWindow(parent), m_pipeline(pipeline)
{
    setWindowTitle("Eclair Look");

    //
    // Setup
    //

    m_logWidget = new LogWidget();
    m_devWidget = new DevWidget(m_pipeline);

    m_tabWidget = new QTabWidget();
    m_tabWidget->addTab(m_devWidget, "Dev");
    m_tabWidget->addTab(m_logWidget, "Log");
    setCentralWidget(m_tabWidget);

    //
    // Actions
    //

    QAction *exportAction = new QAction(QIcon(QPixmap(":/icons/hexa.png")), tr("Export"));

    m_fileMenu = menuBar()->addMenu(tr("&File"));
    m_fileMenu->addAction(exportAction);

    //
    // Connections
    //

    QObject::connect(
        exportAction, &QAction::triggered,
        [this]() {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save 3DLUT"), "", tr("Cube Files (*.cube)"));
            m_pipeline->ExportLUT(fileName.toStdString(), 64);
        }
    );
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
  switch (event->key()) {
      case Qt::Key_Escape:
        close();
        break;
      default:
        QMainWindow::keyPressEvent(event);
  }
}

ImagePipeline *MainWindow::pipeline()
{
    return m_pipeline;
}
