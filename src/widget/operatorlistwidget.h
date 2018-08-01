#pragma once

#include <QtWidgets/QListWidget>


class ImageOperatorList;

class OperatorListWidget : public QListWidget
{
  public:
    OperatorListWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;

    void setOperators(ImageOperatorList *list);

  private:
    ImageOperatorList *m_operators;
};