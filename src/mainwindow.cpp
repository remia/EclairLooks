#include "mainwindow.h"
#include "widget/imagewidget.h"
#include "widget/logwidget.h"
#include "widget/transformationlistwidget.h"
#include "scope/waveformwidget.h"

#include <QtWidgets/QtWidgets>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    //
    // GUI Setup
    //

    m_logWidget = new LogWidget();
    m_imageWidget = new ImageWidget();
    m_transformationsWidget = new TransformationListWidget();
    m_waveformWidget = new WaveformWidget();

    setWindowTitle("Eclair Look");
    setCentralWidget(m_imageWidget);

    QDockWidget *dw = nullptr;

    dw = new QDockWidget("Log");
    dw->setWidget(m_logWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dw);

    dw = new QDockWidget("Scope");
    dw->setWidget(m_waveformWidget);
    addDockWidget(Qt::BottomDockWidgetArea, dw);

    dw = new QDockWidget("Transformations");
    dw->setWidget(m_transformationsWidget);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    QAction *exportAction = new QAction(QIcon(QPixmap(":/icons/hexa.png")), "");

    m_toolBar = new QToolBar();
    m_toolBar->addAction(exportAction);
    addToolBar(Qt::TopToolBarArea, m_toolBar);

    //
    // Connections
    //

    QObject::connect(
        exportAction, &QAction::triggered,
        [this]() {
            QString fileName = QFileDialog::getSaveFileName(this, tr("Save 3DLUT"), "", tr("Cube Files (*.cube)"));
            m_pipeline.ExportLUT(fileName.toStdString(), 64);
        }
    );

    using std::placeholders::_1;
    using IP = ImagePipeline;
    m_pipeline.Subscribe<IP::Evt::Reset>(std::bind(&ImageWidget::setImage, m_imageWidget, _1));
    m_pipeline.Subscribe<IP::Evt::Reset>(std::bind(&WaveformWidget::resetTexture, m_waveformWidget, _1));
    m_pipeline.Subscribe<IP::Evt::Update>(std::bind(&ImageWidget::updateImage, m_imageWidget, _1));

    using IW = ImageWidget;
    m_imageWidget->Subscribe<IW::Evt::Update>(std::bind(&WaveformWidget::updateTexture, m_waveformWidget, _1));
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
