#pragma once

#include <QtWidgets/QWidget>
#include <QtWidgets/QFormLayout>


class ParameterSerialList;

class SettingWidget : public QWidget
{
  public:
    SettingWidget(QWidget *parent = nullptr);

  public:
    void addParameters(ParameterSerialList &settings, const QString &header = "");

  private:
    QFormLayout* m_layout;
};