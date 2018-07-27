#pragma once

#include <QtWidgets/QListWidget>


class QScrollArea;
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
    void setOperatorDetailWidget(QScrollArea *w);

  private:
    void buildFromPipeline();
    void initTransformationWidget(ImageOperator &op);
    void updateSelection(int selectedRow);

  private:
    ImagePipeline *m_pipeline;

    QScrollArea *m_operatorDetailWidget;
};