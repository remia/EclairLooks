#pragma once

#include <QtWidgets/QWidget>

#include "../utils/generic.h"
#include "../utils/event_source.h"


class Parameter;
class TextParameter;
class SelectParameter;
class FilePathParameter;
class CheckBoxParameter;
class SliderParameter;
class Slider;
class QVBoxLayout;
class QHBoxLayout;
class QTextEdit;
class QComboBox;
class QLineEdit;
class QToolButton;
class QCheckBox;

typedef EventDesc<FuncT<void(const Parameter &p)>> PWEvtDesc;

class ParameterWidget : public QWidget, public EventSource<PWEvtDesc>
{
  public:
    enum Evt { Update };

  public:
    ParameterWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateUi(const Parameter &p);
    virtual void UpdateWidget(const Parameter &p) {}

  protected:
    Parameter *m_param;
    QVBoxLayout *m_layout;
};

class ParameterTextWidget : public ParameterWidget
{
  public:
    ParameterTextWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    TextParameter *m_textParam;
    QTextEdit *m_textEdit;
};

class ParameterSelectWidget : public ParameterWidget
{
  public:
    ParameterSelectWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    SelectParameter *m_selectParam;
    QComboBox *m_comboBox;
};

class ParameterFilePathWidget : public ParameterWidget
{
  public:
    ParameterFilePathWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    FilePathParameter *m_filePathParam;
    QLineEdit *m_lineEdit;
    QToolButton *m_toolButton;
};

class ParameterCheckBoxWidget : public ParameterWidget
{
  public:
    ParameterCheckBoxWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    CheckBoxParameter *m_checkBoxParam;
    QCheckBox *m_checkBox;
};

class ParameterSliderWidget : public ParameterWidget
{
  public:
    ParameterSliderWidget(Parameter *param, QWidget *parent = nullptr);

  public:
    void UpdateWidget(const Parameter &p) override;

  private:
    SliderParameter *m_sliderParam;
    QHBoxLayout *m_sliderLayout;
    Slider *m_slider;
    QLineEdit *m_ledit;
};

ParameterWidget* WidgetFromParameter(Parameter *p);
