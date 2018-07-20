#include "transformationwidget.h"
#include "../operator/imageoperator.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>


QWidget * TransformationWidget::FromOperator(ImageOperator & op)
{
    QWidget * widget = new QWidget();
    QVBoxLayout * layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel(QString::fromStdString(op.OpName())));

    for (auto & p : op.Parameters()) {
        QHBoxLayout * rowLayout = new QHBoxLayout();
        rowLayout->addWidget(new QLabel(QString::fromStdString(p->name)));
        rowLayout->addWidget(_WidgetFromParameter(op, *p));

        layout->addLayout(rowLayout);
    }

    return widget;
}

QWidget * TransformationWidget::_WidgetFromParameter(ImageOperator & op, ImageOperatorParameter & p)
{
    switch (p.type) {
        case ImageOperatorParameter::Type::Text:
            return _TextWidget(op, p);
            break;
        case ImageOperatorParameter::Type::Select:
            return _SelectWidget(op, p);
            break;
        case ImageOperatorParameter::Type::FilePath:
            return _FilePathWidget(op, p);
            break;
        case ImageOperatorParameter::Type::CheckBox:
            return _CheckBoxWidget(op, p);
            break;
        case ImageOperatorParameter::Type::Slider:
            return _SliderWidget(op, p);
            break;
        default:
            return new QWidget();
    }
}

QWidget * TransformationWidget::_TextWidget(ImageOperator & op, ImageOperatorParameter & p)
{
    TextParameter * param = static_cast<TextParameter *>(&p);
    QTextEdit * te = new QTextEdit();

    te->setText(QString::fromStdString(param->default_value));

    QObject::connect(
        te, &QTextEdit::textChanged,
        [&, param, te]() {
            param->value = te->toPlainText().toStdString();
            op.SetParameter(*param);
        }
    );

    return te;
}

QWidget * TransformationWidget::_SelectWidget(ImageOperator & op, ImageOperatorParameter & p)
{
    SelectParameter * param = static_cast<SelectParameter *>(&p);
    QComboBox * cb = new QComboBox();

    for (auto & v : param->choices)
        cb->addItem(QString::fromStdString(v));
    cb->setCurrentText(QString::fromStdString(param->default_value));

    QObject::connect(
        cb, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
        [&, param](const QString &text) {
            auto m = AutoMute(&op, ImageOperator::UpdateGui);
            param->value = text.toStdString();
            op.SetParameter(*param);
        }
    );

    op.Subscribe<ImageOperator::UpdateGui>([=](const ImageOperatorParameter & new_p) {
        const SelectParameter * new_param = static_cast<const SelectParameter *>(&new_p);

        if (new_param->name != param->name)
            return;

        cb->clear();
        for (auto & v : new_param->choices) {
            cb->addItem(QString::fromStdString(v));
        }
        cb->setCurrentText(QString::fromStdString(new_param->default_value));
    });

    return cb;
}

QWidget * TransformationWidget::_FilePathWidget(ImageOperator & op, ImageOperatorParameter & p)
{
    FilePathParameter * param = static_cast<FilePathParameter *>(&p);
    QWidget * widget = new QWidget();
    QHBoxLayout * layout = new QHBoxLayout(widget);
    QLineEdit * le = new QLineEdit();
    QToolButton * tb = new QToolButton();
    layout->addWidget(le);
    layout->addWidget(tb);

    op.Subscribe<ImageOperator::UpdateGui>([=](const ImageOperatorParameter & new_p) {
        const FilePathParameter * new_param = static_cast<const FilePathParameter *>(&new_p);
        if (new_param->name != param->name)
            return;

        le->setText(QString::fromStdString(std::any_cast<std::string>(new_param->value)));
    });

    return widget;
}

QWidget * TransformationWidget::_CheckBoxWidget(ImageOperator & op, ImageOperatorParameter & p)
{
    CheckBoxParameter * param = static_cast<CheckBoxParameter *>(&p);
    QCheckBox * cb = new QCheckBox(QString::fromStdString(p.name));
    cb->setCheckState(param->value ? Qt::Checked : Qt::Unchecked);

    QObject::connect(
        cb, QOverload<int>::of(&QCheckBox::stateChanged),
        [&, param](int state) {
            param->value = state == Qt::Checked ? true : false;
            op.SetParameter(*param);
        }
    );

    return cb;
}

QWidget * TransformationWidget::_SliderWidget(ImageOperator & op, ImageOperatorParameter & p)
{
    SliderParameter * param = static_cast<SliderParameter *>(&p);
    QSlider * slider = new QSlider(Qt::Horizontal);
    slider->setMinimum(param->min);
    slider->setMaximum(param->max);
    slider->setSingleStep(param->step);
    slider->setValue(param->value);

    QObject::connect(
        slider, QOverload<int>::of(&QSlider::valueChanged),
        [&, param](int value) {
            param->value = static_cast<float>(value);
            op.SetParameter(*param);
        }
    );

    return slider;
}
