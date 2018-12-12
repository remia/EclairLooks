#pragma once

#include <map>
#include "../utils/generic.h"

#include <QtWidgets/QWidget>


class LookWidget;
class ImageWidget;
class NeutralWidget;
class CubeWidget;
class QLabel;

class LookDetailWidget : public QWidget
{
  public:
    enum class Compare { Selected, Reference };
    using CompareMap = std::map<Compare, QString>;

  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lw);

    void resetView(Compare c);
    void updateView(Compare c);
    void showDetail(const QString &path, Compare c);

  private:
    LookWidget *m_lookWidget;
    ImageWidget *m_imageWidget;
    NeutralWidget *m_neutralsWidget;
    CubeWidget *m_cubeWidget;

    QLabel *m_titleLabel;

    CompareMap m_cmap;
};