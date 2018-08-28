#include "lookdetailwidget.h"
#include "imagewidget.h"
#include "../imagepipeline.h"
#include "../operator/imageoperatorlist.h"

#include <QtWidgets/QtWidgets>


LookDetailWidget::LookDetailWidget(QWidget *parent)
:   QWidget(parent)
{
    m_pipeline = std::make_unique<ImagePipeline>();

    QHBoxLayout *hLayout = new QHBoxLayout(this);
    m_imageWidget = new ImageWidget();
    m_imageWidget->setMinimumSize(0, 0);
    hLayout->addWidget(m_imageWidget);
    m_curveWidget = new QOpenGLWidget();
    hLayout->addWidget(m_curveWidget);
    m_cubeWidget = new QOpenGLWidget();
    hLayout->addWidget(m_cubeWidget);
}

void LookDetailWidget::setPipeline(ImagePipeline *pipeline)
{
    m_globalPipeline = pipeline;
}

void LookDetailWidget::setOperators(ImageOperatorList *list)
{
    m_operators = list;
}

void LookDetailWidget::showDetail(const QString &path)
{
    if (ImageOperator *op = m_operators->CreateFromPath(path.toStdString())) {
        m_pipeline->SetInput(m_globalPipeline->GetInput());
        m_imageWidget->setImage(m_pipeline->GetInput());
        m_pipeline->Reset();
        m_pipeline->AddOperator(op);
        m_imageWidget->updateImage(m_pipeline->GetOutput());
    }
}