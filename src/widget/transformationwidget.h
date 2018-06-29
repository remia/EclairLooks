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
};