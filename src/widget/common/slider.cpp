#include "slider.h"

#include <cmath>

#include <QtWidgets/QtWidgets>


Slider::Slider(Qt::Orientation orientation, Scale s, QWidget *parent)
: QSlider(orientation, parent), m_scale(s)
{
    setMinimumHeight(25);
    setTickInterval(25);
    QSlider::setMinimum(0);
    QSlider::setMaximum(100);
    QSlider::setSingleStep(1);
}

float Slider::value() const
{
    int val = QSlider::value();
    return value(val);
}

float Slider::value(int val) const
{
    switch (m_scale) {
        case Scale::Linear: {
            return std::min(m_min + val * m_step, m_max);
        }
        case Scale::Log: {
            float minv = std::log(m_min);
            float maxv = std::log(m_max);
            float factor = (maxv - minv) / QSlider::maximum();
            return std::exp(minv + factor * (val - QSlider::minimum()));
        }
    }

    return 0.f;
}

void Slider::setValue(float v)
{
    switch (m_scale) {
        case Scale::Linear: {
            int stepcount = (QSlider::maximum() - QSlider::minimum()) / QSlider::singleStep();
            float val = (v - m_min) / (m_max - m_min);
            QSlider::setValue(val * stepcount);
        }
        break;
        case Scale::Log: {
            float minv = std::log(m_min);
            float maxv = std::log(m_max);
            float factor = (maxv - minv) / QSlider::maximum();
            int val = (std::log(v) - minv) / factor + QSlider::minimum();
            QSlider::setValue(val);
        }
        break;
    }
}

void Slider::setMinimum(float v)
{
    m_min = v;
    _update();
}

float Slider::minimum() const
{
    return m_min;
}

void Slider::setMaximum(float v)
{
    m_max = v;
    _update();
}

float Slider::maximum() const
{
    return m_max;
}

void Slider::setSingleStep(float v)
{
    m_step = v;
    _update();
}

float Slider::singleStep() const
{
    return m_step;
}

void Slider::setScale(Scale s)
{
    m_scale = s;
    _update();
}

Slider::Scale Slider::scale() const
{
    return m_scale;
}

void Slider::_update()
{
    if (m_max <= m_min)
        return;
    if (m_step <= 0)
        return;

    switch(m_scale) {
        case Scale::Linear: {
            int requiredStep = (m_max - m_min) / m_step;
            QSlider::setMinimum(0);
            QSlider::setMaximum(requiredStep);
            QSlider::setSingleStep(1);
        }
        break;
        case Scale::Log: {
            QSlider::setMinimum(0);
            QSlider::setMaximum(1000);
            QSlider::setSingleStep(1);
        }
        break;
    }
}

void Slider::paintEvent(QPaintEvent *event)
{
    QSlider::paintEvent(event);

    QPainter painter(this);
    QPen pen;
    pen.setWidth(1);

    uint8_t xmargin = 5;
    uint8_t tick_width = 1;
    uint8_t tick_height = 8;
    uint8_t subtick_count = 5;
    uint8_t subtick_height = 3;

    // Draw legend
    QFont font = painter.font();
    font.setPointSize(font.pointSize() - 3);
    painter.setFont(font);
    QFontMetrics metric(font);

    int min = QSlider::minimum();
    int max = QSlider::maximum();
    int step = QSlider::tickInterval();
    const int maxtick = 4;
    if ((max - min) / step > maxtick)
        step = (max - min) / maxtick;

    for (int v = min; v <= max; v += step) {
        int x = xmargin + width() * (1.f * v / max);
        x = std::min(x, width() - xmargin);

        // Draw ticks above
        pen.setColor(QColor(Qt::darkGray));
        painter.setPen(pen);
        painter.drawRect(x, height() / 2. - tick_height, tick_width, tick_height);

        // Draw intermediate ticks
        pen.setColor(QColor(Qt::black));
        for (int i = 1; i < subtick_count; ++i) {
            int xi = xmargin + width() * ((v + step * (1.f * i / subtick_count)) / max);
            xi = std::min(xi, width() - xmargin);
            painter.drawRect(xi, height() / 2 - subtick_height, tick_width, subtick_height);
        }

        // Draw text below
        QString txt = QString::number(value(v), 'G', 5);
        QRect rect = metric.boundingRect(txt);
        x = x - rect.width() / 2;
        x = std::min(x, width() - rect.width());
        x = std::max(0, x);

        pen.setColor(QColor(Qt::gray));
        painter.setPen(pen);
        painter.drawText(x, height(), txt);
    }
}