#pragma once

#include <QtWidgets/QWidget>

#include "../common/slider.h"


class ParameterSliderWidget : public ParameterWidget
{
  public:
    ParameterSliderWidget(Parameter *param, QWidget *parent = nullptr)
        : ParameterWidget(param, parent)
    {
        m_sliderParam = static_cast<SliderParameter *>(param);

        m_sliderLayout = new QHBoxLayout;
        m_slider = new Slider(Qt::Horizontal);
        m_sliderLayout->addWidget(m_slider);

        m_ledit = new QLineEdit(QString("%1").arg(m_sliderParam->value()));
        m_ledit->setAlignment(Qt::AlignCenter);
        m_ledit->setFixedWidth(85);

        QDoubleValidator *dv = new QDoubleValidator();
        dv->setRange(m_sliderParam->min(), m_sliderParam->max());
        dv->setDecimals(5);
        dv->setNotation(QDoubleValidator::StandardNotation);
        m_ledit->setValidator(dv);

        m_sliderLayout->addWidget(m_ledit);
        m_layout->addLayout(m_sliderLayout);

        UpdateWidget(*param);

        QObject::connect(m_slider, QOverload<int>::of(&QSlider::valueChanged),
                         [&, p = m_sliderParam, le = m_ledit](int value) {
                             p->setValue(m_slider->value());
                             le->setText(QString::number(p->value(), 'G', 5));
                         });

        QObject::connect(m_ledit, QOverload<>::of(&QLineEdit::returnPressed),
                         [&, p1 = m_sliderParam, s = m_slider, le = m_ledit]() {
                             p1->setValue(le->text().toFloat());
                             s->setValue(le->text().toFloat());
                         });
    }

  public:
    void UpdateWidget(const Parameter &p) override
    {
        const SliderParameter *sp = static_cast<const SliderParameter *>(&p);

        m_slider->setMinimum(sp->min());
        m_slider->setMaximum(sp->max());
        m_slider->setSingleStep(sp->step());

        switch (sp->scale()) {
            case SliderParameter::Scale::Linear:
                m_slider->setScale(Slider::Scale::Linear);
                break;
            case SliderParameter::Scale::Log:
                m_slider->setScale(Slider::Scale::Log);
                break;
        }

        m_slider->setValue(sp->value());
    }

  private:
    SliderParameter *m_sliderParam;
    QHBoxLayout *m_sliderLayout;
    Slider *m_slider;
    QLineEdit *m_ledit;
};
