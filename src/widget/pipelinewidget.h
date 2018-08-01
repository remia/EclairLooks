#pragma once

#include <QtWidgets/QListWidget>


class QScrollArea;
class ImagePipeline;
class ImageOperatorList;
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
    void setOperators(ImageOperatorList *list);
    void setOperatorDetailWidget(QScrollArea *w);

  private:
    void buildFromPipeline();
    void initTransformationWidget(ImageOperator &op);

    void updateSelection(QListWidgetItem * item);
    void disableSelection(int selectedRow);
    void removeSelection(int selectedRow);

  private:
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;
    QScrollArea *m_operatorDetailWidget;

    uint16_t m_currentIndex;
};