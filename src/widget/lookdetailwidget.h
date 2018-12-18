#pragma once

#include <map>
#include "../utils/generic.h"
#include "../utils/types.h"

#include <QtWidgets/QWidget>


class LookWidget;
class ImageWidget;
class NeutralWidget;
class CubeWidget;
class QLabel;
class Image;

class LookDetailWidget : public QWidget
{
  public:
    using CompareMap = std::map<SideBySide, QString>;

  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void showEvent(QShowEvent *event) override;

  public:
    void setLookWidget(LookWidget *lw);

    void clearView(SideBySide c);
    void updateView(SideBySide c);
    void showDetail(const QString &path, SideBySide c);

  private:
    LookWidget *m_lookWidget;
    ImageWidget *m_imageWidget;
    NeutralWidget *m_neutralsWidget;
    CubeWidget *m_cubeWidget;

    QLabel *m_titleLabel;

    CompareMap m_cmap;
};