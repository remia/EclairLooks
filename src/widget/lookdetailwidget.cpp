#include "lookdetailwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"
#include "../scope/curvewidget.h"

#include <QtWidgets/QtWidgets>


LookDetailWidget::LookDetailWidget(QWidget *parent)
:   QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);

    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
    m_imageWidget = new ImageWidget();
    hSplitter->addWidget(m_imageWidget);
    m_curveWidget = new CurveWidget();
    hSplitter->addWidget(m_curveWidget);
    m_cubeWidget = new QOpenGLWidget();
    hSplitter->addWidget(m_cubeWidget);

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    hSplitter->setSizes(QList<int>({50000, 50000, 00000}));

    vLayout->addWidget(hSplitter);
}

void LookDetailWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    installEventFilter(m_lookWidget);
}

void LookDetailWidget::resetView()
{
    m_imageWidget->clearImage();
    m_curveWidget->clearView();
}

void LookDetailWidget::showDetail(const QString &path)
{
    if (auto [valid, img] = m_lookWidget->lookPreview(path); valid) {
        m_imageWidget->setImage(m_lookWidget->fullImage());
        m_imageWidget->updateImage(img);
    }
    if (auto [valid, img] = m_lookWidget->lookPreviewRamp(path); valid) {
        m_curveWidget->drawCurves(img);
    }
}