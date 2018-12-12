#pragma once

#include "../utils/generic.h"

#include <QtWidgets/QWidget>


class LookWidget;
class ImageWidget;
class NeutralWidget;
class CubeWidget;

class LookDetailWidget : public QWidget
{
  public:
    enum class Compare { Selected, Reference };

  public:
    LookDetailWidget(QWidget *parent = nullptr);

  public:
    void setLookWidget(LookWidget *lw);

    void resetView(Compare c);
    void showDetail(const QString &path, Compare c);

  private:
    LookWidget *m_lookWidget;
    ImageWidget *m_imageWidget;
    NeutralWidget *m_neutralsWidget;
    CubeWidget *m_cubeWidget;
};