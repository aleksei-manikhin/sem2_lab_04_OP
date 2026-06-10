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

class TouchpadHelper
{
public:
    static constexpr double DefaultScaleFactor = 1.0;
    static constexpr double MoveMinValue = -9999.0;
    static constexpr double MoveMaxValue = 9999.0;
    static constexpr double RotationMinValue = -360.0;
    static constexpr double RotationMaxValue = 360.0;
    static constexpr int ScaleDecimals = 4;
    static constexpr double ScaleSingleStep = 0.001;

    static double deltaToSceneMove(int delta, int isPixelDelta)
    {
        return delta * (isPixelDelta ? MoveByPixel : MoveByAngle);
    }

    static double deltaToZoomFactor(int delta, int isPixelDelta)
    {
        return qBound(
            WheelZoomMinFactor,
            DefaultScaleFactor
                + delta * (isPixelDelta ? WheelZoomByPixel : WheelZoomByAngle),
            WheelZoomMaxFactor);
    }

    static double nativeZoomFactor(double scaleDelta)
    {
        return qBound(
            NativeZoomMinFactor,
            DefaultScaleFactor + scaleDelta,
            NativeZoomMaxFactor);
    }

    static QPoint touchpadDelta(QWheelEvent* wheelEvent, int& isPixelDelta)
    {
        QPoint delta = wheelEvent->pixelDelta();
        isPixelDelta = 1;

        if (delta.isNull()) {
            delta = wheelEvent->angleDelta();
            isPixelDelta = 0;
        }

        return delta;
    }

    static int zScrollDelta(const QPoint& delta)
    {
        return delta.y() != 0 ? delta.y() : delta.x();
    }

private:
    static constexpr double MoveByPixel = 0.12;
    static constexpr double MoveByAngle = 1.6 / 120.0;
    static constexpr double WheelZoomByPixel = 1.0 / 500.0;
    static constexpr double WheelZoomByAngle = 1.0 / 900.0;
    static constexpr double WheelZoomMinFactor = 0.75;
    static constexpr double WheelZoomMaxFactor = 1.35;
    static constexpr double NativeZoomMinFactor = 0.2;
    static constexpr double NativeZoomMaxFactor = 5.0;
};

void MainWindow::setSpinBoxValueWithoutUpdate(
    QDoubleSpinBox* spinBox, double value)
{
    isChangingValues = true;
    spinBox->setValue(value);
    isChangingValues = false;
}

int MainWindow::isTouchpadWidget(const QObject* watched) const
{
    return watched == ui->graphicsView
           || watched == ui->graphicsView->viewport();
}

int MainWindow::handleTouchpadEvent(const QObject* watched, QEvent* event)
{
    if (event == nullptr)
        return 0;

    updateZKeyState(event);

    if (!isTouchpadWidget(watched))
        return 0;
    if (event->type() == QEvent::Wheel)
        return handleTouchpadWheel(static_cast<QWheelEvent*>(event));
    if (event->type() == QEvent::NativeGesture)
        return handleTouchpadNativeGesture(
            static_cast<QNativeGestureEvent*>(event));

    return 0;
}

void MainWindow::updateZKeyState(QEvent* event)
{
    if (event->type() != QEvent::KeyPress
        && event->type() != QEvent::KeyRelease) {
        return;
    }

    QKeyEvent* keyEvent = static_cast<QKeyEvent*>(event);

    if (keyEvent->key() == Qt::Key_Z && !keyEvent->isAutoRepeat())
        isZPressed = event->type() == QEvent::KeyPress;
}

int MainWindow::handleTouchpadWheel(QWheelEvent* wheelEvent)
{
    int isPixelDelta = 1;
    const QPoint delta = TouchpadHelper::touchpadDelta(
        wheelEvent, isPixelDelta);

    if (delta.isNull())
        return 0;

    if (wheelEvent->modifiers() & Qt::ControlModifier)
        zoomTouchpadScene(
            TouchpadHelper::deltaToZoomFactor(delta.y(), isPixelDelta));
    else
        moveTouchpadScene(delta, isPixelDelta);

    wheelEvent->accept();
    return 1;
}

int MainWindow::handleTouchpadNativeGesture(
    QNativeGestureEvent* gestureEvent)
{
    if (gestureEvent->gestureType() != Qt::ZoomNativeGesture)
        return 0;

    const double scaleDelta = gestureEvent->value();

    if (qFuzzyIsNull(scaleDelta))
        return 0;

    zoomTouchpadScene(TouchpadHelper::nativeZoomFactor(scaleDelta));
    gestureEvent->accept();

    return 1;
}

void MainWindow::zoomTouchpadScene(double scaleFactor)
{
    if (!hasLoadedData()
        || qFuzzyCompare(scaleFactor, TouchpadHelper::DefaultScaleFactor)) {
        return;
    }

    const double oldScale = ui->doubleSpinBox_14->value();

    setSpinBoxValueWithoutUpdate(
        ui->doubleSpinBox_14, oldScale * scaleFactor);

    if (scaleFactor > TouchpadHelper::DefaultScaleFactor
        && qFuzzyCompare(ui->doubleSpinBox_14->value(), oldScale)) {
        setSpinBoxValueWithoutUpdate(
            ui->doubleSpinBox_14,
            oldScale + ui->doubleSpinBox_14->singleStep());
    }

    applyTouchpadTransform();
}

void MainWindow::moveTouchpadScene(const QPoint& delta, int isPixelDelta)
{
    if (!hasLoadedData())
        return;

    if (isZPressed)
        moveTouchpadSceneByZ(delta, isPixelDelta);
    else
        moveTouchpadSceneByXY(delta, isPixelDelta);
}

void MainWindow::moveTouchpadSceneByXY(
    const QPoint& delta, int isPixelDelta)
{
    const double oldX = ui->doubleSpinBox_5->value();
    const double oldY = ui->doubleSpinBox_6->value();
    const double dx =
        TouchpadHelper::deltaToSceneMove(delta.x(), isPixelDelta);
    const double dy =
        TouchpadHelper::deltaToSceneMove(delta.y(), isPixelDelta);

    setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_5, oldX + dx);
    setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_6, oldY + dy);
    applyTouchpadTransform();
}

void MainWindow::moveTouchpadSceneByZ(
    const QPoint& delta, int isPixelDelta)
{
    const double oldZ = ui->doubleSpinBox_7->value();
    const double dz = TouchpadHelper::deltaToSceneMove(
        TouchpadHelper::zScrollDelta(delta), isPixelDelta);

    setSpinBoxValueWithoutUpdate(ui->doubleSpinBox_7, oldZ + dz);
    applyTouchpadTransform();
}

void MainWindow::applyTouchpadTransform()
{
    updateTransform(false);
}

void MainWindow::setupTouchpad()
{
    ui->graphicsView->setTransformationAnchor(
        QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    ui->graphicsView->setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->viewport()->setFocusPolicy(Qt::StrongFocus);
    ui->graphicsView->viewport()->unsetCursor();

    ui->doubleSpinBox_5->setRange(
        TouchpadHelper::MoveMinValue, TouchpadHelper::MoveMaxValue);
    ui->doubleSpinBox_6->setRange(
        TouchpadHelper::MoveMinValue, TouchpadHelper::MoveMaxValue);
    ui->doubleSpinBox_7->setRange(
        TouchpadHelper::MoveMinValue, TouchpadHelper::MoveMaxValue);
    ui->doubleSpinBox_8->setRange(
        TouchpadHelper::RotationMinValue, TouchpadHelper::RotationMaxValue);
    ui->doubleSpinBox_9->setRange(
        TouchpadHelper::RotationMinValue, TouchpadHelper::RotationMaxValue);
    ui->doubleSpinBox_10->setRange(
        TouchpadHelper::RotationMinValue, TouchpadHelper::RotationMaxValue);
    ui->doubleSpinBox_14->setDecimals(TouchpadHelper::ScaleDecimals);
    ui->doubleSpinBox_14->setSingleStep(TouchpadHelper::ScaleSingleStep);

    if (QCoreApplication::instance() != nullptr)
        QCoreApplication::instance()->installEventFilter(this);
}
