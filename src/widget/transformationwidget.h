#pragma once


class ImageOperator;
struct ImageOperatorParameter;
class QWidget;

class TransformationWidget
{
public:
    static QWidget * FromOperator(ImageOperator & op);

private:
    static QWidget * _WidgetFromParameter(ImageOperator & op, ImageOperatorParameter & p);
    static QWidget * _TextWidget(ImageOperator & op, ImageOperatorParameter & p);
    static QWidget * _SelectWidget(ImageOperator & op, ImageOperatorParameter & p);
    static QWidget * _FilePathWidget(ImageOperator & op, ImageOperatorParameter & p);
    static QWidget * _CheckBoxWidget(ImageOperator & op, ImageOperatorParameter & p);
    static QWidget * _SliderWidget(ImageOperator & op, ImageOperatorParameter & p);
};