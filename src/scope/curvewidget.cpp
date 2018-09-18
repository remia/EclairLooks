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
    initCursor();
    drawCursor(width() / 2, height() / 2);
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
    for (auto item : m_curveItems)
        m_scene->removeItem(item);
    m_curveItems.clear();

    m_img.reset();
}

void CurveWidget::drawCurves(const Image &img)
{
    clearView();
    m_img.reset(new Image(img));

    // Draw R, G, B curves
    QPen pen;
    QColor color[3] = { Qt::red, Qt::green, Qt::blue };

    const float * pix = img.pixels_asfloat();
    for (uint8_t c = 0; c < 3; ++c) {
        QPainterPath path;
        path.moveTo(0, grid_height - (grid_height * pix[c]));

        for (uint32_t i = 0; i < img.width(); ++i) {
            path.lineTo(
                (1.0 * i / img.width()) * grid_width,
                grid_height - (grid_height * pix[i * 3 + c]));
        }

        pen.setColor(color[c]);
        QGraphicsPathItem *pathItem = m_scene->addPath(path, pen);
        pathItem->setBrush(QColor(0, 0, 0, 0));
        m_curveItems.push_back(pathItem);
    }
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

void CurveWidget::initCursor()
{
    QPen pen;

    pen.setColor(QColor(25, 25, 25));
    m_cursorVLine = m_scene->addLine(QLineF(), pen);

    pen.setColor(QColor(255, 0, 0));
    m_cursorHLineR = m_scene->addLine(QLineF(), pen);
    pen.setColor(QColor(0, 255, 0));
    m_cursorHLineG = m_scene->addLine(QLineF(), pen);
    pen.setColor(QColor(0, 0, 255));
    m_cursorHLineB = m_scene->addLine(QLineF(), pen);

    m_cursorText = m_scene->addText("");
}

void CurveWidget::drawCursor(uint16_t x, uint16_t y)
{
    QPointF scenePos = mapToScene(x, y);

    float inX = scenePos.x() / grid_width;
    inX = std::clamp(inX, 0.0f, 1.0f);
    scenePos.setX(inX * grid_width);

    float outR, outG, outB;
    outR = outG = outB = inX;

    QLineF vLine(scenePos.x(), 0, scenePos.x(), grid_height);
    QLineF hLineR(0, grid_height - scenePos.x(), grid_width, grid_height - scenePos.x());
    QLineF hLineG(0, grid_height - scenePos.x(), grid_width, grid_height - scenePos.x());
    QLineF hLineB(0, grid_height - scenePos.x(), grid_width, grid_height - scenePos.x());

    if (m_img) {
        uint16_t inXInt = std::clamp(
            (int) (inX * m_img->width()), 0, m_img->width() - 1);

        const float * pix = m_img->pixels_asfloat();
        outR = pix[inXInt * 3];
        outG = pix[inXInt * 3 + 1];
        outB = pix[inXInt * 3 + 2];

        hLineR = QLineF(0, grid_height - (outR * grid_height), grid_width, grid_height - (outR * grid_height));
        hLineG = QLineF(0, grid_height - (outG * grid_height), grid_width, grid_height - (outG * grid_height));
        hLineB = QLineF(0, grid_height - (outB * grid_height), grid_width, grid_height - (outB * grid_height));
    }

    m_cursorVLine->setLine(vLine);
    m_cursorHLineR->setLine(hLineR);
    m_cursorHLineG->setLine(hLineG);
    m_cursorHLineB->setLine(hLineB);

    m_cursorText->setHtml(
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
            .arg(QString::number(outR, 'f', 5))
            .arg(QString::number(outG, 'f', 5))
            .arg(QString::number(outB, 'f', 5))
        );

    m_cursorText->setPos(25, grid_height - 25);
}