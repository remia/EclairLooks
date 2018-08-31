#pragma once

#include <QtWidgets/QWidget>


class Settings;

class SettingWidget : public QWidget
{
  public:
    SettingWidget(Settings *settings, QWidget *parent = nullptr);

  private:
    Settings *m_settings;
};