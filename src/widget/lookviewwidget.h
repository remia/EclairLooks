#pragma once

#include "../utils/generic.h"
#include "../utils/event_source.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QTabWidget>

#include <tuple>


class ImagePipeline;
class ImageOperatorList;
class QListWidget;

typedef EventDesc <FuncT<void(const QString &)>> LVEvtDesc;

class LookViewTabWidget : public QTabWidget, public EventSource<LVEvtDesc>
{
  public:
    enum Evt { Select = 0 };

  public:
    LookViewTabWidget(QWidget *parent = nullptr);

  public:
    void setPipeline(ImagePipeline *pipeline);
    void setOperators(ImageOperatorList *list);
    void setBrowserRootPath(const QString &path);

    void showPreview(const QString &path);
    void updateSelection(const QString &path);

  private:
    void tabChanged(int index);
    void tabClosed(int index);
    std::tuple<bool, uint16_t> tabExists(const QString &name);

  private:
    ImagePipeline *m_globalPipeline;
    ImageOperatorList *m_operators;
    QString m_rootPath;
};

class LookViewWidget : public QWidget
{
  public:
    LookViewWidget(QWidget *parent = nullptr);

  public:
    void setLookViewTabWidget(LookViewTabWidget *w);
    void setPipeline(ImagePipeline *pipeline);
    void setOperators(ImageOperatorList *list);

    void showPreview(const QString &path);
    void updateSelection();

  private:
    QWidget *widgetFromLook(const QString &path) const;

  private:
    ImagePipeline *m_globalPipeline;
    UPtr<ImagePipeline> m_pipeline;
    ImageOperatorList *m_operators;

    QListWidget *m_lookList;
    LookViewTabWidget *m_lookViewTabWidget;

    QSize m_thumbSize;
};