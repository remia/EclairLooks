#pragma once

#include <QtWidgets/QWidget>


class LookWidget;
class LookViewTabWidget;
class QListWidget;

class LookViewWidget : public QWidget
{
  public:
    LookViewWidget(QWidget *parent = nullptr);

  public:
    QListWidget *lookListWidget();
    void setLookWidget(LookWidget *lw);
    void setLookViewTabWidget(LookViewTabWidget *w);

    uint16_t countLook() const;

    void showFolder(const QString &path);
    void updateSelection();

  private:
    QWidget *widgetFromLook(const QString &path) const;

  private:
    LookWidget *m_lookWidget;
    LookViewTabWidget *m_lookViewTabWidget;
    QListWidget *m_lookList;
};