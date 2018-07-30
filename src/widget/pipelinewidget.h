#pragma once

#include <QtWidgets/QListWidget>


class QScrollArea;
class ImagePipeline;
class ImageOperator;

class PipelineWidget : public QListWidget
{
  public:
    PipelineWidget(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    QSize sizeHint() const override;

    void setPipeline(ImagePipeline *pipeline);
    void setOperatorDetailWidget(QScrollArea *w);

  private:
    void buildFromPipeline();
    void initTransformationWidget(ImageOperator &op);

    void updateSelection(int selectedRow);
    void disableSelection(int selectedRow);
    void removeSelection(int selectedRow);

  private:
    ImagePipeline *m_pipeline;

    QScrollArea *m_operatorDetailWidget;
};