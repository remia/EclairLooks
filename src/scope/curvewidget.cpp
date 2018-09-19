#include "curvewidget.h"
#include "../utils/gl.h"
#include "../image.h"

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QtWidgets>


uint16_t grid_width = 400;
uint16_t grid_height = 400;

CurveWidget::CurveWidget(QWidget *parent)
: QGraphicsView(parent)
{
    m_scene = new QGraphicsScene();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewport(new QOpenGLWidget());
    setBackgroundBrush(QBrush(QColor(Qt::white)));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setScene(m_scene);

    drawGrid();
}

void CurveWidget::resizeEvent(QResizeEvent *event)
{
    fitInView(m_scene->itemsBoundingRect());
}

void CurveWidget::mouseMoveEvent(QMouseEvent *event)
{
    drawCursor(event->x(), event->y());
}

void CurveWidget::clearView()
{
    for (auto& [k, v] : m_curves)
        clearCurve(k);

    m_curves.clear();
}

void CurveWidget::drawCurve(uint8_t id, const Image &img)
{
    CurveItems curveItems;
    if (auto c = m_curves.find(id); c == m_curves.end()) {
        m_curves[id] = initCurve(id, img);
        curveItems = m_curves[id];
    }
    else {
        m_curves[id].image = img;
        curveItems = m_curves[id];
    }

    drawCurve(curveItems);
}

void CurveWidget::clearCurve(uint8_t id)
{
    if (auto c = m_curves.find(id); c != m_curves.end()) {
        CurveItems & items = c->second;

        m_scene->removeItem(items.curve[0]);
        m_scene->removeItem(items.curve[1]);
        m_scene->removeItem(items.curve[2]);

        m_scene->removeItem(items.cursorHLine[0]);
        m_scene->removeItem(items.cursorHLine[1]);
        m_scene->removeItem(items.cursorHLine[2]);

        m_scene->removeItem(items.cursorVLine);
        m_scene->removeItem(items.cursorText);
        m_curves.erase(id);
    }
}

CurveItems CurveWidget::initCurve(uint8_t id, const Image &img)
{
    CurveItems items;
    QColor colors[3] = { Qt::red, Qt::green, Qt::blue };

    QPen pen;
    if (id == 1)
        pen.setStyle(Qt::DashLine);

    // Curves & Cursor
    for (uint8_t i = 0; i < 3; ++i) {
        pen.setColor(colors[i]);
        items.curve[i] = m_scene->addPath(QPainterPath(), pen);
        items.cursorHLine[i] = m_scene->addLine(QLineF(), pen);
    }

    pen.setColor(QColor(25, 25, 25));
    items.cursorVLine = m_scene->addLine(QLineF(), pen);
    items.cursorText = m_scene->addText("");

    items.image = img;

    return items;
}

void CurveWidget::drawGrid()
{
    QPen pen;
    pen.setColor(QColor(125, 125, 125));
    pen.setWidth(1);

    uint16_t grid_count = 17;

    // Draw Grid
    for (uint16_t i = 0; i <= grid_count; ++i) {
        uint16_t x = i * (1.0 * grid_width / grid_count);
        uint16_t y = i * (1.0 * grid_height / grid_count);
        m_scene->addLine(x, 0, x, grid_width, pen);
        m_scene->addLine(0, y, grid_height, y, pen);
    }

    // Draw identity curve
    m_scene->addLine(0.0, grid_height, grid_width, 0.0, pen);
}

void CurveWidget::drawCurve(const CurveItems &items)
{
    const float * pix = items.image.pixels_asfloat();

    for (uint8_t c = 0; c < 3; ++c) {
        QPainterPath path;
        path.moveTo(0, grid_height - (grid_height * pix[c]));

        for (uint32_t i = 0; i < items.image.width(); ++i) {
            path.lineTo(
                (1.0 * i / items.image.width()) * grid_width,
                grid_height - (grid_height * pix[i * 3 + c]));
        }

        items.curve[c]->setPath(path);
    }
}

void CurveWidget::drawCursor(uint16_t x, uint16_t y)
{
    QPointF scenePos = mapToScene(x, y);

    // Clamp at normalized 0..1 range
    float inX = scenePos.x() / grid_width;
    inX = std::clamp(inX, 0.0f, 1.0f);
    scenePos.setX(inX * grid_width);

    // Draw a cursor for each transforms
    uint8_t count = 0;
    for (auto& [name, items] : m_curves) {

        uint16_t inXInt = std::clamp((int) (inX * items.image.width()), 0, items.image.width() - 1);
        const float * pix = items.image.pixels_asfloat();

        float out[3];
        for (uint8_t i = 0; i < 3; ++i) {
            out[i] = pix[inXInt * 3 + i];
            items.cursorHLine[i]->setLine(QLineF(
                0, grid_height - (out[i] * grid_height),
                grid_width, grid_height - (out[i] * grid_height)));
        }

        items.cursorVLine->setLine(QLineF(
            scenePos.x(), 0, scenePos.x(), grid_height));

        items.cursorText->setHtml(
            QString(
                "<font color=\"red\">%1</font>, "
                "<font color=\"green\">%2</font>, "
                "<font color=\"blue\">%3</font> "
                "->"
                "<font color=\"red\">%4</font>, "
                "<font color=\"green\">%5</font>, "
                "<font color=\"blue\">%6</font> ")
                .arg(QString::number(inX, 'f', 5))
                .arg(QString::number(inX, 'f', 5))
                .arg(QString::number(inX, 'f', 5))
                .arg(QString::number(out[0], 'f', 5))
                .arg(QString::number(out[1], 'f', 5))
                .arg(QString::number(out[2], 'f', 5))
            );

        items.cursorText->setPos(25, grid_height - 25 - count * 25);

        count++;
    }
}
