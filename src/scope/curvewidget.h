#pragma once

#include "../utils/generic.h"

#include <vector>

#include <QtWidgets/QGraphicsView>


class Image;
class QGraphicsScene;
class QGraphicsView;
class QGraphicsItem;
class QGraphicsLineItem;
class QGraphicsTextItem;

class CurveWidget : public QGraphicsView
{
  public:
    CurveWidget(QWidget *parent = nullptr);

  public:
    void resizeEvent(QResizeEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

  public:
    void clearView();
    void drawCurves(const Image &img);

  private:
    void drawGrid();
    void initCursor();
    void drawCursor(uint16_t x, uint16_t y);

  private:
    UPtr<Image> m_img;

    QGraphicsScene *m_scene;
    std::vector<QGraphicsItem*> m_curveItems;
    QGraphicsLineItem *m_cursorVLine;
    QGraphicsLineItem *m_cursorHLineR;
    QGraphicsLineItem *m_cursorHLineG;
    QGraphicsLineItem *m_cursorHLineB;
    QGraphicsTextItem *m_cursorText;
};