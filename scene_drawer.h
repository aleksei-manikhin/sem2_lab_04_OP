#ifndef SCENE_DRAWER_H
#define SCENE_DRAWER_H

#include "scene.h"

#include <QGraphicsScene>
#include <QPointF>

class QColor;
class QGraphicsView;
class QPen;
class QString;

class SceneDrawer
{
public:
    explicit SceneDrawer(QGraphicsView& graphicsView);

    void drawScene(const Scene& scene, bool fitScene);

private:
    static constexpr double ProjectionScale = 35.0;

    QGraphicsView& graphicsView;
    QGraphicsScene graphicsScene;

    QPointF projectPoint(const Point3D& point) const;
    QPen createPen(const QColor& color, double width) const;
    void drawSurface(const Surface& surface, const QPen& pen);
    void drawEdge(const Edge3D& edge, const QPen& pen);
    void drawAxis(const Edge3D& axis, const QPen& pen, const QString& label);
};

#endif // SCENE_DRAWER_H
