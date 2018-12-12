#pragma once

#include <QtWidgets/QWidget>


class ParameterSelectWidget : public ParameterWidget
{
  public:
    ParameterSelectWidget(Parameter *param, QWidget *parent = nullptr)
        : ParameterWidget(param, parent)
    {
        m_selectParam = static_cast<SelectParameter *>(param);

        m_comboBox = new QComboBox();
        m_layout->addWidget(m_comboBox);

        UpdateWidget(*param);

        QObject::connect(m_comboBox,
                         QOverload<const QString &>::of(&QComboBox::activated),
                         [&, p = m_selectParam](const QString &text) {
                             p->setValue(text.toStdString());
                         });
    }

  public:
    void UpdateWidget(const Parameter &p) override
    {
        const SelectParameter *sp = static_cast<const SelectParameter *>(&p);

        m_comboBox->clear();
        for (auto &v : sp->choices())
            m_comboBox->addItem(QString::fromStdString(v));

        if (!sp->value().empty())
            m_comboBox->setCurrentText(QString::fromStdString(sp->value()));
        else
            m_comboBox->setCurrentText(QString::fromStdString(sp->defaultValue()));
    }

  private:
    SelectParameter *m_selectParam;
    QComboBox *m_comboBox;
};
