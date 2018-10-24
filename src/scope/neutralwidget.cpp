#include "neutralwidget.h"
#include "../utils/gl.h"
#include "../image.h"

#include <QtWidgets/QGraphicsScene>
#include <QtWidgets/QOpenGLWidget>
#include <QtWidgets/QtWidgets>


uint16_t grid_width = 640;
uint16_t grid_height = 400;

QColor backgroundColor = QColor(39, 39, 44);
QColor gridSmallColor = QColor(55, 56, 61);
QColor gridLargeColor = QColor(69, 72, 77);
QColor lineYColor = QColor(200, 200, 200);
QColor lineRColor = QColor(200, 25, 25);
QColor lineGColor = QColor(25, 200, 25);
QColor lineBColor = QColor(25, 25, 200);

NeutralWidget::NeutralWidget(QWidget *parent)
: QGraphicsView(parent)
{
    m_scene = new QGraphicsScene();

    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setViewport(new QOpenGLWidget());
    setBackgroundBrush(QBrush(backgroundColor));
    setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
    setScene(m_scene);

    drawGrid();
}

void NeutralWidget::resizeEvent(QResizeEvent *event)
{
    fitInView(m_scene->itemsBoundingRect(), Qt::KeepAspectRatio);
}

void NeutralWidget::mouseMoveEvent(QMouseEvent *event)
{
    drawCursor(event->x(), event->y());
}

void NeutralWidget::clearView()
{
    for (auto& [k, v] : m_curves)
        clearCurve(k);

    m_curves.clear();
}

void NeutralWidget::drawCurve(uint8_t id, const Image &img)
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

void NeutralWidget::clearCurve(uint8_t id)
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

CurveItems NeutralWidget::initCurve(uint8_t id, const Image &img)
{
    CurveItems items;
    QColor colors[3] = { lineRColor, lineGColor, lineBColor };

    QPen pen;
    pen.setWidth(2);
    if (id == 1)
        pen.setStyle(Qt::DashLine);

    // Curves & Cursor
    for (uint8_t i = 0; i < 3; ++i) {
        pen.setColor(colors[i]);
        items.curve[i] = m_scene->addPath(QPainterPath(), pen);
        items.cursorHLine[i] = m_scene->addLine(QLineF(), pen);
    }

    pen.setColor(lineYColor);
    items.cursorVLine = m_scene->addLine(QLineF(), pen);
    items.cursorText = m_scene->addText("");

    items.image = img;

    return items;
}

void NeutralWidget::drawGrid()
{
    QPen pen;
    pen.setWidth(1);

    uint16_t grid_count = 5;

    // Draw Grid
    for (uint16_t i = 0; i <= grid_count; ++i) {
        pen.setColor(gridLargeColor);
        uint16_t xi = i * (1.0 * grid_width / grid_count);
        uint16_t yi = i * (1.0 * grid_height / grid_count);
        m_scene->addLine(xi, 0, xi, grid_height, pen);
        m_scene->addLine(0, yi, grid_width, yi, pen);

        if (i < grid_count) {
            pen.setColor(gridSmallColor);
            for (uint16_t j = 1; j < grid_count; ++j) {
                uint16_t xj = xi + j * (1.0 * grid_width / (grid_count * grid_count));
                uint16_t yj = yi + j * (1.0 * grid_height / (grid_count * grid_count));
                m_scene->addLine(xj, 0, xj, grid_height, pen);
                m_scene->addLine(0, yj, grid_width, yj, pen);
            }
        }
    }

    // Draw identity curve
    m_scene->addLine(0.0, grid_height, grid_width, 0.0, pen);
}

void NeutralWidget::drawCurve(const CurveItems &items)
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

void NeutralWidget::drawCursor(uint16_t x, uint16_t y)
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
                "%1 - %2"
                "=> "
                "<font color=\"red\">%3</font> "
                "<font color=\"green\">%4</font> "
                "<font color=\"blue\">%5</font> ")
                .arg(name)
                .arg(QString::number(inX, 'f', 5))
                .arg(QString::number(out[0], 'f', 5))
                .arg(QString::number(out[1], 'f', 5))
                .arg(QString::number(out[2], 'f', 5))
            );

        items.cursorText->setPos(25, grid_height - 25 - count * 25);

        count++;
    }
}
