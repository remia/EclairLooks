#pragma once

#include <QtWidgets/QWidget>


class ImagePipeline;
class ImageWidget;
class TransformationListWidget;
class WaveformWidget;

class DevWidget : public QWidget
{
  public:
    DevWidget(ImagePipeline *pipeline, QWidget *parent = nullptr);

  private:
    QWidget * setupUi();
    void initPipelineView();

  private:
    ImagePipeline *m_pipeline;

    ImageWidget *m_imageWidget;
    TransformationListWidget *m_transformationsWidget;
    WaveformWidget *m_waveformWidget;
};