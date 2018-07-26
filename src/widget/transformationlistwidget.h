#pragma once

#include <QtWidgets/QListWidget>


class ImagePipeline;
class ImageOperator;

class TransformationListWidget : public QListWidget
{
  public:
    TransformationListWidget(QWidget *parent = nullptr);

  public:
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    QSize sizeHint() const override;

    void setPipeline(ImagePipeline *pipeline);

  private:
    void initTransformationWidget(ImageOperator &op);

  private:
    ImagePipeline *m_pipeline;
};