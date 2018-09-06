#include "lookwidget.h"
#include "uiloader.h"
#include "lookbrowserwidget.h"
#include "lookviewwidget.h"
#include "lookdetailwidget.h"
#include "../settings.h"
#include "../image.h"
#include "../imagepipeline.h"
#include "../mainwindow.h"
#include "../operator/imageoperatorlist.h"
#include "../operator/ociofiletransform_operator.h"

#include <QtWidgets/QtWidgets>
#include <QFile>

using std::placeholders::_1;
using LB = LookBrowserWidget;
using LV = LookViewTabWidget;


LookWidget::LookWidget(MainWindow *mw, QWidget *parent)
    : QWidget(parent), m_mainWindow(mw), m_proxySize(160, 160)
{
    m_pipeline = std::make_unique<ImagePipeline>();

    //
    // Setup
    //

    QWidget * w = setupUi();
    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(w);
    setLayout(layout);

    m_browserWidget = findChild<LookBrowserWidget*>("lookBrowserWidget");
    m_viewWidget = findChild<LookViewTabWidget*>("lookViewWidget");
    m_detailWidget = findChild<LookDetailWidget*>("lookDetailWidget");
    m_browserSearch = findChild<QLineEdit*>("lookBrowserSearch");

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    QSplitter *vSplitter = findChild<QSplitter*>("hSplitter");
    vSplitter->setSizes(QList<int>({15000, 85000}));
    QSplitter *hSplitter = findChild<QSplitter*>("vSplitter");
    hSplitter->setSizes(QList<int>({75000, 25000}));

    setupPipeline();

    m_browserWidget->setLookWidget(this);
    m_viewWidget->setLookWidget(this);
    m_detailWidget->setLookWidget(this);

    //
    // Connections
    //

    QObject::connect(m_browserSearch, &QLineEdit::textChanged, m_browserWidget, &LookBrowserWidget::filterList);

    m_browserWidget->Subscribe<LB::Select>(std::bind(&LookViewTabWidget::showPreview, m_viewWidget, _1));
    m_viewWidget->Subscribe<LV::Reset>(std::bind(&LookDetailWidget::resetView, m_detailWidget));
    m_viewWidget->Subscribe<LV::Select>(std::bind(&LookDetailWidget::showDetail, m_detailWidget, _1));
}

QString LookWidget::rootPath()
{
    std::string val = m_mainWindow->settings()->GetParameter<FilePathParameter>("Look Base Folder").value;
    return QString::fromStdString(val);
}

QString LookWidget::tonemapPath()
{
    std::string val = m_mainWindow->settings()->GetParameter<FilePathParameter>("Look Tonemap LUT").value;
    return QString::fromStdString(val);
}

Image & LookWidget::fullImage()
{
    return *m_image;
}

Image & LookWidget::proxyImage()
{
    return *m_imageProxy;
}

TupleT<bool, Image &>LookWidget::lookPreview(const QString &lookPath)
{
    return _lookPreview(lookPath, fullImage());
}

TupleT<bool, Image &> LookWidget::lookPreviewProxy(const QString &lookPath)
{
    return _lookPreview(lookPath, proxyImage());
}

TupleT<bool, Image &> LookWidget::_lookPreview(const QString &lookPath, Image &img)
{
    if (auto op = m_mainWindow->operators()->CreateFromPath(lookPath.toStdString()); op != nullptr) {
        m_pipeline->ReplaceOperator(op, 0);
        m_pipeline->SetInput(img);
        return { true, m_pipeline->GetOutput() };
    }

    return { false, img };
}

QWidget * LookWidget::setupUi()
{
    UiLoader loader;
    QFile file(":/ui/lookwidget.ui");
    file.open(QFile::ReadOnly);
    return loader.load(&file, this);
}

void LookWidget::setupPipeline()
{
    m_image = std::make_unique<Image>(m_mainWindow->pipeline()->GetInput());
    m_imageProxy = std::make_unique<Image>(*m_image);
    *m_imageProxy = m_imageProxy->resize(m_proxySize.width(), m_proxySize.height());

    m_pipeline->AddOperator<OCIOFileTransform>();

    if (!tonemapPath().isEmpty()) {
        if (auto op = m_mainWindow->operators()->CreateFromPath(tonemapPath().toStdString()); op != nullptr) {
            m_pipeline->AddOperator(op);
        }
    }
}