#include "widget.h"
#include "graphic.h"
#include "parameter.h"

#include <gui/common/common.h>
#include <gui/common/slider.h>
#include <parameter/slider/widget.h>


ParameterCurveEditWidget::ParameterCurveEditWidget(Parameter *param, QWidget *parent)
: ParameterWidget(param, parent)
{
    m_curveEditParam = static_cast<CurveEditParameter *>(param);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);

    m_scene = new CurveScene();
    m_view = new CurveView();
    m_view->setScene(m_scene);
    hLayout->addWidget(m_view);

    QVBoxLayout *vLayout = new QVBoxLayout();

    // Controls
    m_selectLayout = new QHBoxLayout();
    m_selectLayout->setContentsMargins(0, 0, 0, 0);
    m_selectLayout->setSpacing(4);

    m_selectLayout->addStretch(1);

    QPushButton *selectBtn = new QPushButton(QIcon(QPixmap(":/icons/chain.png")), "");
    selectBtn->setCheckable(true);
    selectBtn->setFixedSize(20, 20);
    m_selectBtns = new QButtonGroup(this);
    m_selectBtns->addButton(selectBtn, 0);
    m_selectLayout->addWidget(selectBtn);

    m_selectLayout->addStretch(1);

    QPushButton *resetBtn = new QPushButton(QIcon(QPixmap(":/icons/reset.png")), "");
    resetBtn->setFixedSize(20, 20);
    m_resetBtns = new QButtonGroup(this);
    m_resetBtns->addButton(resetBtn, 0);
    m_selectLayout->addWidget(resetBtn);

    vLayout->addLayout(m_selectLayout);

    m_curveLayout = new QVBoxLayout();
    m_curveLayout->setContentsMargins(0, 0, 0, 0);
    m_curveLayout->setSpacing(2);
    m_curveLayout->addStretch(1);
    vLayout->addLayout(m_curveLayout);

    hLayout->addLayout(vLayout);

    m_layout->addLayout(hLayout);

    // QObject::connect(m_selectBtns, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ControlWidget::selectCurve);
    // QObject::connect(m_resetBtns, QOverload<int>::of(&QButtonGroup::buttonClicked), this, &ControlWidget::resetCurve);
    // QObject::connect(m_view, &CurveView::selectedCurve, this, &ControlWidget::selectCurve);

    initWidget();
    updateWidget(*param);
}

void ParameterCurveEditWidget::updateWidget(const Parameter &p)
{

}

void ParameterCurveEditWidget::initWidget()
{
    for (auto &cp : m_curveEditParam->curves()) {
        addCurve(cp.get());
        m_scene->addCurve(cp->shortName(), cp->knots(), toQColor(cp->color()));
    }

    if (m_curveEditParam->backgroundColorCb())
        m_view->setBackgroundColorCb(*m_curveEditParam->backgroundColorCb());
    else
        m_view->setBackgroundColor(toQColor(m_curveEditParam->backgroundColor()));
}

// void ParameterCurveEditWidget::updateCurve(int id, const Curve &curve)
// {
    // int index = m_plotSelect->currentIndex();
    // if (index >= m_plots.size())
    //     return;

    // CurvePlot &cp = m_plots[index];
    // if (id == -1) {
    //     for (int i = 0; i < cp.curves.size(); ++i)
    //         cp.curves[i] = curve;
    // }
    // else {
    //     cp.curves[id] = curve;
    // }

    // m_curveEditParam->setCurvePlot(index, cp);
// }

void ParameterCurveEditWidget::addCurve(CurveParameter *curve)
{
    QColor color = toQColor(curve->color());

    // Select controls
    QPushButton *selectBtn = new QPushButton(QString::fromStdString(curve->shortName()));
    selectBtn->setCheckable(true);
    selectBtn->setFixedSize(20, 20);
    selectBtn->setStyleSheet(QString(R"(
        QPushButton:checked { background: %1; color: black; }
        )").arg(color.name()));
    m_selectBtns->addButton(selectBtn, m_selectBtns->buttons().count());
    m_selectLayout->insertWidget(m_selectLayout->count() - 2, selectBtn);

    // Curve control
    QHBoxLayout *layout = new QHBoxLayout();

    // Mix
    ParameterSliderWidget *sld = static_cast<ParameterSliderWidget*>(curve->opacityParameter()->createWidget());
    sld->sliderField()->setShowGradation(false);
    sld->sliderField()->setStyleSheet(QString(R"(
        QSlider::sub-page:horizontal { background: %1; }
        QSlider::add-page:horizontal { background: %2; }
        )").arg(color.name()).arg(color.darker().name()));
    layout->addWidget(sld);

    // Reset
    QPushButton *resetBtn = new QPushButton(QIcon(QPixmap(":/icons/reset.png")), "");
    resetBtn->setFixedSize(20, 20);
    m_resetBtns->addButton(resetBtn, m_resetBtns->buttons().count());
    layout->addWidget(resetBtn);

    m_curveLayout->insertLayout(m_curveLayout->count() - 1, layout);
}

void ParameterCurveEditWidget::selectCurve(int index)
{
    // if (index == 0) {
    //     m_selectBtns->setExclusive(false);
    //     for (auto btn : m_selectBtns->buttons())
    //         btn->setChecked(true);
    // }
    // else {
    //     for (auto btn : m_selectBtns->buttons())
    //         btn->setChecked(false);
    //     m_selectBtns->setExclusive(true);
    //     m_selectBtns->buttons()[index]->setChecked(true);
    // }

    // m_view->selectCurve(index-1);
}

void ParameterCurveEditWidget::mixCurve(int index, float v)
{
    // qInfo() << "Mix" << index << v;

    // if (index == -1) {
    //     for (int i = 0; i < m_plot.curves.size(); ++i) {
    //         m_plot.curves[i].opacity = v;
    //         m_widget->updateCurve(i, m_plot.curves[i]);
    //     }
    // }
    // else {
    //     m_plot.curves[index].opacity = v;
    //     m_widget->updateCurve(index, m_plot.curves[index]);
    // }
}

void ParameterCurveEditWidget::resetCurve(int index)
{
    // qInfo() << "Reset" << index;

    // if (index == 0) {
    //     for (int i = 0; i < m_plot.curves.size(); ++i) {
    //         Curve c = m_plot.curves[i];
    //         c.points = {
    //             { 0.0, 0.0, 45.0, 1.0 },
    //             { 1.0, 1.0, 45.0, 1.0 },
    //         };
    //         qInfo() << "A" << index;
    //         m_view->updateCurve(i, c);
    //         qInfo() << "B" << index;
    //         // m_widget->updateCurve(i, c);
    //         qInfo() << "C" << index;
    //     }
    // }
    // else {
    //     Curve c = m_plot.curves[index-1];
    //     c.points = {
    //         { 0.0, 0.0, 45.0, 1.0 },
    //         { 1.0, 1.0, 45.0, 1.0 },
    //     };
    //     m_view->updateCurve(index-1, c);
    //     // m_widget->updateCurve(index-1, c);
    // }
    // qInfo() << "END Reset" << index;
}
