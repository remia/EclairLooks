#pragma once

#include <QtWidgets/QWidget>


class Settings;

class SettingWidget : public QWidget
{
  public:
    SettingWidget(Settings *settings, const QString &header = "",
                  QWidget *parent = nullptr);

  private:
    Settings *m_settings;
    QString m_headerName;
};