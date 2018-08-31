#pragma once

#include <QtWidgets/QWidget>


class ImageOperator;

class OperatorWidget : public QWidget
{
  public:
    OperatorWidget(ImageOperator *op, QWidget *parent = nullptr);

  private:
    void setupUi();

  private:
    ImageOperator *m_operator;
};