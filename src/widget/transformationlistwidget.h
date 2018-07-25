#pragma once

#include <QtWidgets/QListWidget>


class ImagePipeline;
class ImageOperator;

class TransformationListWidget : public QListWidget
{
  public:
    TransformationListWidget(ImagePipeline *pipeline, QWidget *parent = nullptr);

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    QSize sizeHint() const override;

  private:
    void initTransformationWidget(ImageOperator &op);

  private:
    ImagePipeline *m_pipeline;
};