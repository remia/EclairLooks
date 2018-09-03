#include "lookdetailwidget.h"
#include "lookwidget.h"
#include "imagewidget.h"

#include <QtWidgets/QtWidgets>


LookDetailWidget::LookDetailWidget(QWidget *parent)
:   QWidget(parent)
{
    QHBoxLayout *hLayout = new QHBoxLayout(this);
    m_imageWidget = new ImageWidget();
    m_imageWidget->setMinimumSize(0, 0);
    hLayout->addWidget(m_imageWidget);
    m_curveWidget = new QOpenGLWidget();
    hLayout->addWidget(m_curveWidget);
    m_cubeWidget = new QOpenGLWidget();
    hLayout->addWidget(m_cubeWidget);
}

void LookDetailWidget::setLookWidget(LookWidget *lw)
{
    m_lookWidget = lw;
}

void LookDetailWidget::showDetail(const QString &path)
{
    if (auto [valid, img] = m_lookWidget->lookPreview(path); valid) {
        m_imageWidget->setImage(m_lookWidget->fullImage());
        m_imageWidget->updateImage(img);
    }
}