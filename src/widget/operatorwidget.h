#pragma once


class ImageOperator;
struct Parameter;
class QWidget;

class OperatorWidget
{
public:
    static QWidget * FromOperator(ImageOperator & op);

private:
    static QWidget * _WidgetFromParameter(ImageOperator & op, Parameter & p);
    static QWidget * _TextWidget(ImageOperator & op, Parameter & p);
    static QWidget * _SelectWidget(ImageOperator & op, Parameter & p);
    static QWidget * _FilePathWidget(ImageOperator & op, Parameter & p);
    static QWidget * _CheckBoxWidget(ImageOperator & op, Parameter & p);
    static QWidget * _SliderWidget(ImageOperator & op, Parameter & p);
};