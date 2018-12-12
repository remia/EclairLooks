#pragma once

#include <QtWidgets/QWidget>


class ParameterFilePathWidget : public ParameterWidget
{
  public:
    ParameterFilePathWidget(Parameter *param, QWidget *parent = nullptr)
        : ParameterWidget(param, parent)
    {
        m_filePathParam = static_cast<FilePathParameter *>(param);

        QHBoxLayout *hLayout = new QHBoxLayout();
        hLayout->setContentsMargins(0, 0, 0, 0);

        m_lineEdit = new QLineEdit();
        m_toolButton = new QToolButton();
        hLayout->addWidget(m_lineEdit);
        hLayout->addWidget(m_toolButton);
        m_layout->addLayout(hLayout);

        UpdateWidget(*param);

        QObject::connect(m_toolButton, &QToolButton::clicked,
                         [&, p = m_filePathParam, w = this, le = m_lineEdit]() {
                             QString path = QDir(QString::fromStdString(p->value())).path();
                             QString fileName;

                             if (p->pathType() == FilePathParameter::PathType::File)
                                 fileName = QFileDialog::getOpenFileName(
                                     w, QString::fromStdString(p->description()), path,
                                     QString::fromStdString(p->filters()));
                             else
                                 fileName = QFileDialog::getExistingDirectory(
                                     w, QString::fromStdString(p->description()), path);

                             if (!fileName.isEmpty()) {
                                 p->setValue(fileName.toStdString());
                                 le->setText(QString::fromStdString(p->value()));
                             }
                         });

        QObject::connect(m_lineEdit, &QLineEdit::editingFinished,
                         [&, p = m_filePathParam, le = m_lineEdit]() {
                             std::string currentText = le->text().toStdString();
                             if (currentText != p->value()) {
                                 p->setValue(currentText);
                             }
                         });
    }

  public:
    void UpdateWidget(const Parameter &p) override
    {
        const FilePathParameter *fpp = static_cast<const FilePathParameter *>(&p);

        m_lineEdit->setText(QString::fromStdString(fpp->value()));
        m_toolButton->setText("...");
    }

  private:
    FilePathParameter *m_filePathParam;
    QLineEdit *m_lineEdit;
    QToolButton *m_toolButton;
};
