#pragma once

#include <QtWidgets/QMainWindow>


class Settings;
class ImagePipeline;
class ImageOperatorList;
class DevWidget;
class LookWidget;
class LogWidget;
class SettingWidget;

class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

  public:
    void keyPressEvent(QKeyEvent *event) override;
    QSize sizeHint() const override;

  public:
    void setup();

    void setPipeline(ImagePipeline *p);
    ImagePipeline *pipeline();

    void setOperators(ImageOperatorList *l);
    ImageOperatorList *operators();

    void setSettings(Settings *s);
    Settings *settings();

  private:
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;
    Settings *m_settings;

    QMenu *m_fileMenu;

    QTabWidget *m_tabWidget;
    LogWidget *m_logWidget;
    DevWidget *m_devWidget;
    LookWidget *m_lookWidget;
    SettingWidget *m_settingWidget;
};