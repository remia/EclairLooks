#pragma once

#include <string>

#include <QtGui/QKeySequence>

#include "../parameter.h"


class ShortcutParameter : public Parameter
{
  public:
    ShortcutParameter() = default;
    ShortcutParameter(const std::string &name, const std::string& display_name);
    ShortcutParameter(const std::string &name, const std::string& display_name, const QKeySequence &seq);

  public:
    QKeySequence value() const;
    void setValue(const QKeySequence &v);

  public:
    ParameterWidget *newWidget(QWidget * parent = nullptr) override;

    void load(const QSettings *setting) override;
    void save(QSettings *setting) const override;

  private:
    QKeySequence m_value;
};