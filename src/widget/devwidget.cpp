#include "devwidget.h"
#include "uiloader.h"
#include "transformationlistwidget.h"
#include "imagewidget.h"
#include "../scope/waveformwidget.h"
#include "../imagepipeline.h"

#include <QtWidgets/QtWidgets>
#include <QFile>


DevWidget::DevWidget(ImagePipeline *pipeline, QWidget *parent)
    : QWidget(parent), m_pipeline(pipeline)
{
    UiLoader loader;
    QFile file(":/ui/devwidget.ui");
    file.open(QFile::ReadOnly);
    QWidget *myWidget = loader.load(&file, this);
    file.close();

    QVBoxLayout *layout = new QVBoxLayout();
    layout->addWidget(myWidget);
    setLayout(layout);

    m_imageWidget = findChild<ImageWidget*>("imageWidget");
    m_transformationsWidget = findChild<TransformationListWidget*>("pipelineWidget");
    m_waveformWidget = findChild<WaveformWidget*>("waveformWidget");

    m_transformationsWidget->setPipeline(m_pipeline);

    using std::placeholders::_1;
    using IP = ImagePipeline;
    using IW = ImageWidget;

    m_pipeline->Subscribe<IP::Reset>(std::bind(&ImageWidget::setImage, m_imageWidget, _1));
    m_pipeline->Subscribe<IP::Update>(std::bind(&ImageWidget::updateImage, m_imageWidget, _1));

    m_pipeline->Subscribe<IP::Reset>(std::bind(&WaveformWidget::resetTexture, m_waveformWidget, _1));
    m_imageWidget->Subscribe<IW::Update>(std::bind(&WaveformWidget::updateTexture, m_waveformWidget, _1));
}
