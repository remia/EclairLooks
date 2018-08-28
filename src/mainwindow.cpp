#include "mainwindow.h"
#include "imagepipeline.h"
#include "widget/devwidget.h"
#include "widget/lookwidget.h"
#include "widget/logwidget.h"

#include <QtWidgets/QtWidgets>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_pipeline(nullptr), m_operators(nullptr)
{
    setWindowTitle("Eclair Look");
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

QSize MainWindow::sizeHint() const
{
    return QSize(1280, 800);
}

void MainWindow::setup()
{
    if (!m_pipeline || !m_operators)
        return;

    //
    // Setup
    //

    m_logWidget = new LogWidget();
    m_devWidget = new DevWidget(m_pipeline, m_operators);
    m_lookWidget = new LookWidget(m_pipeline, m_operators);

    m_tabWidget = new QTabWidget();
    m_tabWidget->addTab(m_devWidget, "Dev");
    m_tabWidget->addTab(m_lookWidget, "Look");
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

void MainWindow::setPipeline(ImagePipeline *p)
{
    m_pipeline = p;
}

ImagePipeline *MainWindow::pipeline()
{
    return m_pipeline;
}

void MainWindow::setOperators(ImageOperatorList *l)
{
    m_operators = l;
}

ImageOperatorList *MainWindow::operators()
{
    return m_operators;
}

