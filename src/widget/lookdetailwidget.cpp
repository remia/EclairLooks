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
    hSplitter->setSizes(QList<int>({58000, 42000, 00000}));

    vLayout->addWidget(hSplitter);
}

void LookDetailWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    installEventFilter(m_lookWidget);
    m_curveWidget->installEventFilter(m_lookWidget);
}

void LookDetailWidget::resetView(uint8_t id)
{
    m_imageWidget->clearImage();
    m_curveWidget->clearCurve(id);
}

void LookDetailWidget::showDetail(const QString &path, uint8_t id)
{
    if (auto [valid, img] = m_lookWidget->lookPreview(path); valid) {
        m_imageWidget->setImage(m_lookWidget->fullImage());
        m_imageWidget->updateImage(img);
    }
    if (auto [valid, img] = m_lookWidget->lookPreviewRamp(path); valid) {
        m_curveWidget->drawCurve(id, img);
    }
}