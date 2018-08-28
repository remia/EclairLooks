#pragma once

#include "../utils/generic.h"
#include "../utils/event_source.h"

#include <QtWidgets/QWidget>


class ImagePipeline;
class ImageOperatorList;
class QListWidget;

typedef EventDesc <
    FuncT<void(const QString &)>> LVEvtDesc;

class LookViewWidget : public QWidget, public EventSource<LVEvtDesc>
{
  public:
    enum Evt { Select = 0 };

  public:
    LookViewWidget(QWidget *parent = nullptr);

  public:
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
};