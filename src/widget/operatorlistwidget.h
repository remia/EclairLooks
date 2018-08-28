#pragma once

#include <QtWidgets/QListWidget>


class ImageOperatorList;

class OperatorListWidget : public QListWidget
{
  public:
    OperatorListWidget(QWidget *parent = nullptr);

  public:
    void mousePressEvent(QMouseEvent *event) override;

  public:
    void setOperators(ImageOperatorList *list);

  private:
    ImageOperatorList *m_operators;
};