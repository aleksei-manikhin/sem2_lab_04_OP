#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "entrypoint.h"

#include <QMainWindow>
#include <QString>

#include <memory>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class QDoubleSpinBox;
class QDropEvent;
class QEvent;
class QMimeData;
class QNativeGestureEvent;
class QPoint;
class QWheelEvent;
class SceneDrawer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private:
    Ui::MainWindow *ui;
    AppContext context;
    std::unique_ptr<SceneDrawer> sceneDrawer;
    QString currentFilePath;
    bool isChangingValues = false;
    bool isZPressed = false;

    void setupSceneDrawer();
    void setupConnections();
    void setupDragAndDrop();
    void setupTouchpad();
    void onChooseFileClicked();
    void onResetButtonClicked();
    void onSurfaceParametersChanged();
    void onTransformParametersChanged();
    void selectFile(const QString& filePath);
    SurfaceParams surfaceParamsFromUi() const;
    TransformParams transformParamsFromUi() const;
    bool loadData(const QString& filePath);
    bool rebuildSurface(bool fitScene);
    bool updateTransform(bool fitScene);
    bool handleOperationResult(bool fitScene);
    QString statusText(Status status) const;
    int hasLoadedData() const;

    int isDropWidget(const QObject* watched) const;
    int isTouchpadWidget(const QObject* watched) const;
    int handleDragDropEvent(const QObject* watched, QEvent* event);
    int handleTouchpadEvent(const QObject* watched, QEvent* event);
    void updateZKeyState(QEvent* event);
    int handleTouchpadWheel(QWheelEvent* wheelEvent);
    int handleTouchpadNativeGesture(QNativeGestureEvent* gestureEvent);
    void zoomTouchpadScene(double scaleFactor);
    void moveTouchpadScene(const QPoint& delta, int isPixelDelta);
    void moveTouchpadSceneByXY(const QPoint& delta, int isPixelDelta);
    void moveTouchpadSceneByZ(const QPoint& delta, int isPixelDelta);
    void applyTouchpadTransform();
    int acceptDropEvent(QDropEvent* dropEvent, int shouldSelectFile);
    void setSpinBoxValueWithoutUpdate(QDoubleSpinBox* spinBox, double value);
    void setDropHintVisible(int isVisible);
    QString droppedFilePath(const QMimeData* mimeData) const;
};
#endif // MAINWINDOW_H
