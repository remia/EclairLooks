#include "widget.h"
#include "parameter.h"


ParameterShortcutWidget::ParameterShortcutWidget(Parameter *param, QWidget *parent)
: ParameterWidget(param, parent)
{
    m_shortcutParam = static_cast<ShortcutParameter *>(param);

    QHBoxLayout *hLayout = new QHBoxLayout();
    hLayout->setContentsMargins(0, 0, 0, 0);

    m_keyEdit = new QKeySequenceEdit();
    hLayout->addWidget(m_keyEdit);
    m_layout->addLayout(hLayout);

    updateWidget(*param);

    QObject::connect(m_keyEdit, &QKeySequenceEdit::keySequenceChanged,
                     [&, p = m_shortcutParam, te = m_keyEdit]() {
                         p->setValue(te->keySequence());
                     });
}

void ParameterShortcutWidget::updateWidget(const Parameter &p)
{
    const ShortcutParameter *sp = static_cast<const ShortcutParameter *>(&p);
    m_keyEdit->setKeySequence(sp->value());
}
