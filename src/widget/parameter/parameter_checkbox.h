#pragma once

#include <QtWidgets/QWidget>


class ParameterCheckBoxWidget : public ParameterWidget
{
  public:
    ParameterCheckBoxWidget(Parameter *param, QWidget *parent = nullptr)
        : ParameterWidget(param, parent)
    {
        m_checkBoxParam = static_cast<CheckBoxParameter *>(param);

        m_checkBox = new QCheckBox();
        m_layout->addWidget(m_checkBox);

        UpdateWidget(*param);

        QObject::connect(m_checkBox, &QCheckBox::clicked,
                         [&, p = m_checkBoxParam, cb = m_checkBox]() {
                             p->setValue(cb->isChecked() ? true : false);
                         });
    }

  public:
    void UpdateWidget(const Parameter &p) override
    {
        const CheckBoxParameter *cbp = static_cast<const CheckBoxParameter *>(&p);
        m_checkBox->setCheckState(cbp->value() ? Qt::Checked : Qt::Unchecked);
    }

  private:
    CheckBoxParameter *m_checkBoxParam;
    QCheckBox *m_checkBox;
};
