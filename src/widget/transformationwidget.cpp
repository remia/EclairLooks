#include "transformationwidget.h"
#include "../operator/imageoperator.h"

#include <QtWidgets/QtWidgets>


QWidget * TransformationWidget::FromOperator(ImageOperator & op)
{
    QWidget * widget = new QWidget();
    QVBoxLayout * layout = new QVBoxLayout(widget);
    layout->addWidget(new QLabel(QString::fromStdString(op.OpName())));

    for (auto & p : op.Parameters()) {
        QHBoxLayout * rowLayout = new QHBoxLayout();
        rowLayout->addWidget(new QLabel(QString::fromStdString(p.name)));
        rowLayout->addWidget(_WidgetFromParameter(op, p));
        layout->addLayout(rowLayout);
    }

    return widget;
}

QWidget * TransformationWidget::_WidgetFromParameter(ImageOperator & op, ImageOperatorParameter & p)
{
    switch (p.type) {
        case ImageOperatorParameter::Type::Text: {
            QTextEdit * te = new QTextEdit();
            if (p.default_val.has_value())
                te->setText(QString::fromStdString(std::any_cast<std::string>(p.default_val)));

            QObject::connect(
                te, &QTextEdit::textChanged,
                [&, p, te]() {
                    op.Parameters().Set(p.name, te->toPlainText().toStdString());
                }
            );

            return te;
        }
        break;
        case ImageOperatorParameter::Type::Select: {
            QComboBox * cb = new QComboBox();
            if (p.values.has_value()) {
                auto vals = std::any_cast<std::vector<std::string>>(p.values);
                for (auto & v : vals)
                    cb->addItem(QString::fromStdString(v));
            }

            int idx = 0;
            if (p.default_val.has_value()) {
                auto default_val = std::any_cast<std::string>(p.default_val);
                idx = default_val != "" ? std::stoi(default_val) : 0;
            }
            cb->setCurrentIndex(idx);

            QObject::connect(
                cb, QOverload<const QString &>::of(&QComboBox::currentIndexChanged),
                [&, p](const QString &text) {
                    op.Parameters().Set(p.name, text.toStdString());
                }
            );

            using IOPL = ImageOperatorParameterList;
            op.Parameters().Subscribe<IOPL::UpdateParam>([=](const ImageOperatorParameter & new_p) {
                if (new_p.name != p.name)
                    return;
                cb->clear();
                auto vals = std::any_cast<std::vector<std::string>>(new_p.values);
                for (auto & v : vals)
                    cb->addItem(QString::fromStdString(v));

                int idx = 0;
                if (new_p.default_val.has_value()) {
                    auto new_val = std::any_cast<std::string>(new_p.default_val);
                    idx = new_val != "" ? std::stoi(new_val) : 0;
                }
                cb->setCurrentIndex(idx);
            });

            return cb;
        }
        break;
        case ImageOperatorParameter::Type::FilePath: {
            QWidget * widget = new QWidget();
            QHBoxLayout * layout = new QHBoxLayout(widget);
            QLineEdit * le = new QLineEdit();
            QToolButton * tb = new QToolButton();
            layout->addWidget(le);
            layout->addWidget(tb);

            using IOPL = ImageOperatorParameterList;
            op.Parameters().Subscribe<IOPL::UpdateParam>([=](const ImageOperatorParameter & new_p) {
                if (new_p.name != p.name)
                    return;
                le->setText(QString::fromStdString(std::any_cast<std::string>(new_p.value)));
            });

            return widget;
        }
        break;
        case ImageOperatorParameter::Type::CheckBox: {
            QCheckBox * cb = new QCheckBox(QString::fromStdString(p.name));
            cb->setCheckState(std::any_cast<bool>(p.value) ? Qt::Checked : Qt::Unchecked);

            QObject::connect(
                cb, QOverload<int>::of(&QCheckBox::stateChanged),
                [&, p](int state) {
                    op.Parameters().Set(p.name, state == Qt::Checked ? true : false);
                }
            );

            return cb;
        }
        break;
        case ImageOperatorParameter::Type::Slider: {
            QSlider * slider = new QSlider(Qt::Horizontal);
            auto params = std::any_cast<std::vector<float>>(p.values);
            slider->setMinimum(params[0]);
            slider->setMaximum(params[1]);
            slider->setSingleStep(params[2]);

            if (p.value.has_value()) {
                auto value = std::any_cast<float>(p.value);
                slider->setValue(value);
            }

            QObject::connect(
                slider, QOverload<int>::of(&QSlider::valueChanged),
                [&, p](int value) {
                    op.Parameters().Set(p.name, static_cast<float>(value));
                }
            );

            return slider;
        }
        break;
        default:
            return new QWidget();
    }
}