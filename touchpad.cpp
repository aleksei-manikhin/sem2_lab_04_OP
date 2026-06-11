#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QEvent>
#include <QGraphicsView>
#include <QKeyEvent>
#include <QNativeGestureEvent>
#include <QPoint>
#include <QtGlobal>
#include <QWheelEvent>

static const double DEFAULT_SCALE_FACTOR = 1.0;
static const double MOVE_BY_PIXEL = 0.12;
static const double MOVE_BY_ANGLE = 1.6 / 120.0;
static const double WHEEL_ZOOM_BY_PIXEL = 1.0 / 500.0;
static const double WHEEL_ZOOM_BY_ANGLE = 1.0 / 900.0;
static const double WHEEL_ZOOM_MIN_FACTOR = 0.75;
static const double WHEEL_ZOOM_MAX_FACTOR = 1.35;
static const double NATIVE_ZOOM_MIN_FACTOR = 0.2;
static const double NATIVE_ZOOM_MAX_FACTOR = 5.0;

void MainWindow::setSpinBoxValueWithoutUpdate(QDoubleSpinBox* spinBox, double value)
{
    isChangingValues = true;
    spinBox->setValue(value);
    isChangingValues = false;
}

int MainWindow::isTouchpadWidget(const QObject* watched) const
{
    return watched == ui->graphicsView || watched == ui->graphicsView->viewport();
}

int MainWindow::handleTouchpadEvent(const QObject* watched, QEvent* event)
{
    int isHandled = 0;

    if (event != nullptr) {
        updateZKeyState(event);

        if (isTouchpadWidget(watched)) {
            if (event->type() == QEvent::Wheel)
                isHandled = handleTouchpadWheel(static_cast<QWheelEvent*>(event));
            else if (event->type() == QEvent::NativeGesture)
                isHandled = handleTouchpadNativeGesture(static_cast<QNativeGestureEvent*>(event));
        }
    }

    return isHandled;
}

void MainWindow::updateZKeyState(QEvent* event)
{
    const int isKeyEvent = event->type() == QEvent::KeyPress
                           || event->type() == QEvent::KeyRelease;

    if (isKeyEvent) {
        QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);
        if (keyEvent->key() == Qt::Key_Z && !keyEvent->isAutoRepeat())
            isZPressed = event->type() == QEvent::KeyPress;
    }
}

int MainWindow::handleTouchpadWheel(QWheelEvent* wheelEvent)
{
    int isHandled = 0;
    int isPixelDelta = 1;
    QPoint delta = wheelEvent->pixelDelta();

    if (delta.isNull()) {
        delta = wheelEvent->angleDelta();
        isPixelDelta = 0;
    }
    if (!delta.isNull()) {
        if (wheelEvent->modifiers() & Qt::ControlModifier) {
            const double zoomStep = isPixelDelta ? WHEEL_ZOOM_BY_PIXEL : WHEEL_ZOOM_BY_ANGLE;
            const double scaleFactor = qBound(WHEEL_ZOOM_MIN_FACTOR, DEFAULT_SCALE_FACTOR + delta.y() * zoomStep,
                                              WHEEL_ZOOM_MAX_FACTOR);
            zoomTouchpadScene(scaleFactor);
        } else
            moveTouchpadScene(delta, isPixelDelta);

        wheelEvent->accept();
        isHandled = 1;
    }

    return isHandled;
}

int MainWindow::handleTouchpadNativeGesture(QNativeGestureEvent* gestureEvent)
{
    int isHandled = 0;
    const double scaleDelta = gestureEvent->value();
    const int isZoom = gestureEvent->gestureType() == Qt::ZoomNativeGesture;

    if (isZoom && !qFuzzyIsNull(scaleDelta)) {
        const double scaleFactor = qBound(NATIVE_ZOOM_MIN_FACTOR,
                                          DEFAULT_SCALE_FACTOR + scaleDelta,
                                          NATIVE_ZOOM_MAX_FACTOR);
        zoomTouchpadScene(scaleFactor);
        gestureEvent->accept();
        isHandled = 1;
    }

    return isHandled;
}

void MainWindow::zoomTouchpadScene(double scaleFactor)
{
    const int shouldZoom = hasLoadedData()
                           && !qFuzzyCompare(scaleFactor, DEFAULT_SCALE_FACTOR);

    if (shouldZoom) {
        const double oldScale = ui->doubleSpinBox_14->value();
        setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_14, oldScale * scaleFactor);

        if (scaleFactor > DEFAULT_SCALE_FACTOR
            && qFuzzyCompare(ui->doubleSpinBox_14->value(), oldScale))
            setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_14,
                                         oldScale + ui->doubleSpinBox_14->singleStep());

        updateTransform(false);
    }
}

void MainWindow::moveTouchpadScene(const QPoint& delta, int isPixelDelta)
{
    const double moveStep = isPixelDelta ? MOVE_BY_PIXEL : MOVE_BY_ANGLE;

    if (hasLoadedData()) {
        if (isZPressed) {
            const int zDelta = delta.y() != 0 ? delta.y() : delta.x();
            setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_7,
                                         ui->doubleSpinBox_7->value() + zDelta * moveStep);
        } else {
            setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_5,
                                         ui->doubleSpinBox_5->value() + delta.x() * moveStep);
            setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_6,
                                         ui->doubleSpinBox_6->value() + delta.y() * moveStep);
        }

        updateTransform(false);
    }
}

void MainWindow::setupTouchpad()
{
    ui->graphicsView->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->viewport()->setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->viewport()->unsetCursor();

    if (QCoreApplication::instance() != nullptr)
        QCoreApplication::instance()->installEventFilter(this);
}

