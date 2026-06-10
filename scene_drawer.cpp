#include "scene_drawer.h"

#include <QColor>
#include <QFont>
#include <QGraphicsTextItem>
#include <QGraphicsView>
#include <QPainter>
#include <QPen>
#include <QRectF>
#include <QString>

SceneDrawer::SceneDrawer(QGraphicsView& graphicsView)
    : graphicsView(graphicsView)
{
    graphicsView.setScene(&graphicsScene);
    graphicsView.setRenderHint(QPainter::Antialiasing);
}

void SceneDrawer::drawScene(const Scene& scene, bool fitScene)
{
    graphicsScene.clear();

    drawAxis(scene.axes[0], createPen(QColor(220, 63, 63), 3.0), "X");
    drawAxis(scene.axes[1], createPen(QColor(50, 160, 85), 3.0), "Y");
    drawAxis(scene.axes[2], createPen(QColor(50, 105, 210), 3.0), "Z");
    drawSurface(scene.surface, createPen(QColor(37, 76, 134), 2.2));

    if (fitScene) {
        const QRectF sceneRect =
            graphicsScene.itemsBoundingRect().adjusted(-25.0, -25.0, 25.0, 25.0);
        graphicsScene.setSceneRect(sceneRect);
        graphicsView.fitInView(sceneRect, Qt::KeepAspectRatio);
    }
}

QPointF SceneDrawer::projectPoint(const Point3D& point) const
{
    const double screenX = (point.x - point.y) * ProjectionScale;
    const double screenY =
        ((point.x + point.y) * 0.5 - point.z) * ProjectionScale;

    return QPointF(screenX, screenY);
}

QPen SceneDrawer::createPen(const QColor& color, double width) const
{
    QPen pen(color, width);
    pen.setCosmetic(true);

    return pen;
}

void SceneDrawer::drawSurface(const Surface& surface, const QPen& pen)
{
    if (surface.points == nullptr)
        return;

    for (size_t row = 0; row < surface.rows; row++)
        for (size_t column = 0; column < surface.columns; column++) {
            const size_t index = row * surface.columns + column;

            if (column + 1 < surface.columns) {
                const Edge3D horizontalEdge = {
                    surface.points[index],
                    surface.points[index + 1]
                };
                drawEdge(horizontalEdge, pen);
            }
            if (row + 1 < surface.rows) {
                const Edge3D verticalEdge = {
                    surface.points[index],
                    surface.points[index + surface.columns]
                };
                drawEdge(verticalEdge, pen);
            }
        }
}

void SceneDrawer::drawEdge(const Edge3D& edge, const QPen& pen)
{
    const QPointF begin = projectPoint(edge.begin);
    const QPointF end = projectPoint(edge.end);

    graphicsScene.addLine(begin.x(), begin.y(), end.x(), end.y(), pen);
}

void SceneDrawer::drawAxis(const Edge3D& axis, const QPen& pen, const QString& label)
{
    drawEdge(axis, pen);

    const QPointF labelPosition = projectPoint(axis.end);
    QGraphicsTextItem* text =
        graphicsScene.addText(label, QFont("Segoe UI", 10, QFont::Bold));
    text->setDefaultTextColor(pen.color());
    text->setPos(labelPosition.x() + 6.0, labelPosition.y() - 18.0);
}
