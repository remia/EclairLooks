#include "lookdetailwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"
#include "../scope/neutralwidget.h"
#include "../scope/cubewidget.h"

#include <QtWidgets/QtWidgets>


LookDetailWidget::LookDetailWidget(QWidget *parent)
:   QWidget(parent)
{
    QVBoxLayout *vLayout = new QVBoxLayout(this);
    vLayout->setContentsMargins(0, 0, 0, 0);

    QSplitter *hSplitter = new QSplitter(Qt::Horizontal);
    m_imageWidget = new ImageWidget();
    hSplitter->addWidget(m_imageWidget);
    m_neutralsWidget = new NeutralWidget();
    hSplitter->addWidget(m_neutralsWidget);
    m_cubeWidget = new CubeWidget();
    hSplitter->addWidget(m_cubeWidget);

    // NOTE : see https://stackoverflow.com/a/43835396/4814046
    hSplitter->setSizes(QList<int>({33333, 33333, 33333}));

    m_titleLabel = new QLabel();
    m_titleLabel->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);

    vLayout->addWidget(m_titleLabel);
    vLayout->addWidget(hSplitter);
}

void LookDetailWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
    installEventFilter(m_lookWidget);
    m_neutralsWidget->installEventFilter(m_lookWidget);
}

void LookDetailWidget::resetView(Compare c)
{
    m_cmap[c] = "";
    m_titleLabel->setText(m_cmap[Compare::Selected]);

    m_imageWidget->clearImage();
    m_neutralsWidget->clearCurve(UnderlyingT<Compare>(c));
    m_cubeWidget->resetCube();
}
}

void LookDetailWidget::showDetail(const QString &path, Compare c)
{
    m_cmap[c] = path;
    m_titleLabel->setText(m_cmap[Compare::Selected]);

    if (auto [valid, img] = m_lookWidget->lookPreview(path); valid) {
        m_imageWidget->setImage(m_lookWidget->fullImage());
        m_imageWidget->updateImage(img);
    }
    if (auto [valid, img] = m_lookWidget->lookPreviewRamp(path); valid) {
        m_neutralsWidget->drawCurve(UnderlyingT<Compare>(c), img, path);
    }
    if (auto [valid, img] = m_lookWidget->lookPreviewLattice(path); valid) {
        m_cubeWidget->drawCube(img);
    }
}