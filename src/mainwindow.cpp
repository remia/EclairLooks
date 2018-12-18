#include "mainwindow.h"
#include "imagepipeline.h"
#include "settings.h"
#include "version.h"
#include "widget/devwidget.h"
#include "widget/lookwidget.h"
#include "widget/logwidget.h"
#include "widget/settingwidget.h"

#include <QtWidgets/QtWidgets>


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), m_pipeline(nullptr), m_operators(nullptr), m_settings(nullptr)
{
    setWindowTitle(ELOOK_VERSION_PRETTY);

    QSettings settings;
    restoreGeometry(settings.value("mw/geometry").toByteArray());
    restoreState(settings.value("mw/windowState").toByteArray());
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (QMessageBox::Yes == QMessageBox::question(
        this, "Close Confirmation", "Are you sure to quit ?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes)) {
        QSettings settings;
        settings.setValue("mw/geometry", saveGeometry());
        settings.setValue("mw/windowState", saveState());
        QMainWindow::closeEvent(event);

        event->accept();
        return;
    }

    event->ignore();
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
    m_devWidget = new DevWidget(this);
    m_lookWidget = new LookWidget(this);
    m_settingWidget = new SettingWidget(m_settings, "General");

    m_tabWidget = new QTabWidget();
    m_tabWidget->addTab(m_devWidget, "Dev");
    m_tabWidget->addTab(m_lookWidget, "Look");
    m_tabWidget->addTab(m_settingWidget, "Setting");
    m_tabWidget->addTab(m_logWidget, "Log");
    setCentralWidget(m_tabWidget);

    setupHelp();

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

void MainWindow::setSettings(Settings *s)
{
    m_settings = s;
}

Settings *MainWindow::settings()
{
    return m_settings;
}

void MainWindow::setupHelp()
{
    QString html;
    QFile f = QFile(":/html/help.html");
    if (f.open(QIODevice::ReadOnly)) {
        QTextStream in(&f);
        html = in.readAll();
    }

    QTextEdit * helpWidget = new QTextEdit();
    helpWidget->setHtml(html);
    helpWidget->setReadOnly(true);

    m_tabWidget->addTab(helpWidget, "Help");
}