#pragma once

#include <QtWidgets/QMainWindow>


class ParameterSerialList;
class ImagePipeline;
class ImageOperatorList;
class DevWidget;
class LookWidget;
class LogWidget;
class SettingWidget;

// Main gui class holding the different tabs of the application
// TODO : must somewhere make a global application context (singleton)
// enabling easy access to settings and other global parameters.
class MainWindow : public QMainWindow
{
  public:
    MainWindow(QWidget *parent = nullptr);

  public:
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    QSize sizeHint() const override;

  public:
    void setup();
    void centerOnScreen();

    void setPipeline(ImagePipeline *p);
    ImagePipeline *pipeline();

    void setOperators(ImageOperatorList *l);
    ImageOperatorList *operators();

    void setSettings(ParameterSerialList *s);
    ParameterSerialList *settings();

    QString lookBasePath() const;
    QString imageBasePath() const;
    QString tonemapPath() const;
    QStringList supportedLookExtensions();

  private:
    void setupHelp();

  private:
    ImagePipeline *m_pipeline;
    ImageOperatorList *m_operators;
    ParameterSerialList *m_settings;

    QMenu *m_fileMenu;

    QTabWidget *m_tabWidget;
    LogWidget *m_logWidget;
    DevWidget *m_devWidget;
    LookWidget *m_lookWidget;
    SettingWidget *m_settingWidget;
};