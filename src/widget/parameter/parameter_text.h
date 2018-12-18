#pragma once

#include <QtWidgets/QWidget>


class ParameterTextWidget : public ParameterWidget
{
  public:
    ParameterTextWidget(Parameter *param, QWidget *parent = nullptr)
    : ParameterWidget(param, parent)
    {
        m_textParam = static_cast<TextParameter *>(param);

        m_textEdit = new QTextEdit();
        m_layout->addWidget(m_textEdit);

        UpdateWidget(*param);

        QObject::connect(m_textEdit, &QTextEdit::textChanged,
                         [&, p = m_textParam, te = m_textEdit]() {
                             p->setValue(te->toPlainText().toStdString());
                         });
    }

  public:
    void UpdateWidget(const Parameter &p) override
    {
        const TextParameter *tp = static_cast<const TextParameter *>(&p);
        m_textEdit->setText(QString::fromStdString(tp->defaultValue()));
    }

  private:
    TextParameter *m_textParam;
    QTextEdit *m_textEdit;
};