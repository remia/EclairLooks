#include "parameterwidget.h"
#include "../parameter/parameter.h"

#include <QtWidgets/QtWidgets>
#include <QtCore/QDebug>


// ----------------------------------------------------------------------------

ParameterWidget::ParameterWidget(Parameter *param, QWidget *parent)
:   QWidget(parent), m_param(param), m_layout(nullptr)
{
    m_layout = new QVBoxLayout(this);
    m_layout->setContentsMargins(0, 0, 0, 0);
}

void ParameterWidget::UpdateUi(const Parameter &p)
{
    if (p.name != m_param->name)
        return;

    UpdateWidget(p);
}

// ----------------------------------------------------------------------------

ParameterTextWidget::ParameterTextWidget(Parameter *param, QWidget *parent)
:   ParameterWidget(param, parent)
{
    m_textParam = static_cast<TextParameter*>(param);

    m_textEdit = new QTextEdit();
    m_layout->addWidget(m_textEdit);

    UpdateWidget(*param);

    QObject::connect(
        m_textEdit, &QTextEdit::textChanged,
        [&, p = m_textParam, te = m_textEdit]() {
            p->value = te->toPlainText().toStdString();
            EmitEvent<Update>(*p);
        }
    );
}

void ParameterTextWidget::UpdateWidget(const Parameter &p)
{
    const TextParameter *tp = static_cast<const TextParameter*>(&p);
    m_textEdit->setText(QString::fromStdString(tp->default_value));
}

// ----------------------------------------------------------------------------

ParameterSelectWidget::ParameterSelectWidget(Parameter *param, QWidget *parent)
:   ParameterWidget(param, parent)
{
    m_selectParam = static_cast<SelectParameter*>(param);

    m_comboBox = new QComboBox();
    m_layout->addWidget(m_comboBox);

    UpdateWidget(*param);

    QObject::connect(
        m_comboBox, QOverload<const QString &>::of(&QComboBox::activated),
        [&, p = m_selectParam](const QString &text) {
            p->value = text.toStdString();
            EmitEvent<Update>(*p);
        }
    );
}

void ParameterSelectWidget::UpdateWidget(const Parameter &p)
{
    const SelectParameter *sp = static_cast<const SelectParameter*>(&p);

    m_comboBox->clear();
    for (auto & v : sp->choices)
        m_comboBox->addItem(QString::fromStdString(v));

    if (!sp->value.empty())
        m_comboBox->setCurrentText(QString::fromStdString(sp->value));
    else
        m_comboBox->setCurrentText(QString::fromStdString(sp->default_value));
}

// ----------------------------------------------------------------------------

ParameterFilePathWidget::ParameterFilePathWidget(Parameter *param, QWidget *parent)
:   ParameterWidget(param, parent)
{
    m_filePathParam = static_cast<FilePathParameter*>(param);

    QHBoxLayout * hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);

    m_lineEdit = new QLineEdit();
    m_toolButton = new QToolButton();
    hLayout->addWidget(m_lineEdit);
    hLayout->addWidget(m_toolButton);
    m_layout->addLayout(hLayout);

    UpdateWidget(*param);

    QObject::connect(
        m_toolButton, &QToolButton::clicked,
        [&, p = m_filePathParam, w = this, le = m_lineEdit]() {
            QString fileName;

            if (p->path_type == FilePathParameter::PathType::File)
                fileName = QFileDialog::getOpenFileName(w, QString::fromStdString(p->dialog_title), "", QString::fromStdString(p->filters));
            else
                fileName = QFileDialog::getExistingDirectory(w, QString::fromStdString(p->dialog_title));

            if (!fileName.isEmpty()) {
                p->value = fileName.toStdString();
                le->setText(QString::fromStdString(p->value));
                EmitEvent<Update>(*p);
            }
        }
    );

    QObject::connect(
        m_lineEdit, &QLineEdit::editingFinished,
        [&, p = m_filePathParam, le = m_lineEdit]() {
            std::string currentText = le->text().toStdString();
            if (currentText != p->value) {
                p->value = currentText;
                EmitEvent<Update>(*p);
            }
        }
    );
}

void ParameterFilePathWidget::UpdateWidget(const Parameter &p)
{
    const FilePathParameter *fpp = static_cast<const FilePathParameter*>(&p);

    m_lineEdit->setText(QString::fromStdString(fpp->value));
    m_toolButton->setText("...");
}

// ----------------------------------------------------------------------------

ParameterCheckBoxWidget::ParameterCheckBoxWidget(Parameter *param, QWidget *parent)
:   ParameterWidget(param, parent)
{
    m_checkBoxParam = static_cast<CheckBoxParameter*>(param);

    m_checkBox = new QCheckBox(QString::fromStdString(m_checkBoxParam->name));
    m_layout->addWidget(m_checkBox);

    UpdateWidget(*param);

    QObject::connect(
        m_checkBox, &QCheckBox::clicked,
        [&, p = m_checkBoxParam, cb = m_checkBox]() {
            p->value = cb->isChecked() ? true : false;
            EmitEvent<Update>(*p);
        }
    );

}

void ParameterCheckBoxWidget::UpdateWidget(const Parameter &p)
{
    const CheckBoxParameter *cbp = static_cast<const CheckBoxParameter*>(&p);
    m_checkBox->setCheckState(cbp->value ? Qt::Checked : Qt::Unchecked);
}

// ----------------------------------------------------------------------------

ParameterSliderWidget::ParameterSliderWidget(Parameter *param, QWidget *parent)
:   ParameterWidget(param, parent)
{
    m_sliderParam = static_cast<SliderParameter*>(param);

    m_slider = new QSlider(Qt::Horizontal);
    m_slider->setTickPosition(QSlider::TicksBelow);
    m_layout->addWidget(m_slider);

    UpdateWidget(*param);

    QObject::connect(
        m_slider, QOverload<int>::of(&QSlider::valueChanged),
        [&, p = m_sliderParam](int value) {
            p->value = static_cast<float>(value);
            EmitEvent<Update>(*p);
        }
    );
}

void ParameterSliderWidget::UpdateWidget(const Parameter &p)
{
    const SliderParameter *sp = static_cast<const SliderParameter*>(&p);
    m_slider->setMinimum(sp->min);
    m_slider->setMaximum(sp->max);
    m_slider->setSingleStep(sp->step);
    m_slider->setValue(sp->value);
}

// ----------------------------------------------------------------------------

ParameterWidget* WidgetFromParameter(Parameter *p)
{
    switch (p->type) {
        case Parameter::Type::Text:
            return new ParameterTextWidget(p);
            break;
        case Parameter::Type::Select:
            return new ParameterSelectWidget(p);
            break;
        case Parameter::Type::FilePath:
            return new ParameterFilePathWidget(p);
            break;
        case Parameter::Type::CheckBox:
            return new ParameterCheckBoxWidget(p);
            break;
        case Parameter::Type::Slider:
            return new ParameterSliderWidget(p);
            break;
        default:
            return new ParameterWidget(p);
    }
}