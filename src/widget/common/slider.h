#pragma once

#include <QtWidgets/QSlider>


class Slider : public QSlider
{
  public:
    enum class Scale { Linear, Log };

  public:
    Slider(Qt::Orientation, Scale = Scale::Linear, QWidget *parent = 0);

  public:
    float value() const;
    float value(int val) const;
    void setValue(float v);

    void setMinimum(float v);
    float minimum() const;
    void setMaximum(float v);
    float maximum() const;
    void setSingleStep(float v);
    float singleStep() const;
    void setScale(Scale s);
    Scale scale() const;

  protected:
    void paintEvent(QPaintEvent *event);

  private:
    void _update();

  private:
    float m_min = 0;
    float m_max = 100;
    float m_step = 1;
    Scale m_scale = Scale::Linear;
};
