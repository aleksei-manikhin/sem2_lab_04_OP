#include "mainwindow.h"
#include "scene_drawer.h"
#include "ui_mainwindow.h"

#include <QCoreApplication>
#include <QFileDialog>
#include <QStatusBar>

#include <string>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("CSV Surface Viewer");
    setWindowIcon(ui->icon_2->icon());

    doOperation(INITIALIZE, &context, nullptr);
    setupSceneDrawer();
    setupConnections();
    setupDragAndDrop();
    setupTouchpad();

    ui->contentStackedWidget->setCurrentWidget(ui->emptyPage);
    ui->statusbar->showMessage("Выберите CSV-файл");
}

MainWindow::~MainWindow()
{
    doOperation(DISPOSE, &context, nullptr);
    delete ui;
}

void MainWindow::setupSceneDrawer()
{
    sceneDrawer = std::make_unique<SceneDrawer>(*ui->graphicsView);
}

void MainWindow::setupConnections()
{
    connect(ui->icon_2, &QToolButton::clicked, this, &MainWindow::onChooseFileClicked);
    connect(ui->icon_4, &QToolButton::clicked, this, &MainWindow::onChooseFileClicked);
    connect(ui->pushButton, &QPushButton::clicked, this, &MainWindow::onResetButtonClicked);

    connect(ui->doubleSpinBox_2, &QDoubleSpinBox::valueChanged, this, &MainWindow::onSurfaceParametersChanged);
    connect(ui->doubleSpinBox, &QDoubleSpinBox::valueChanged, this, &MainWindow::onSurfaceParametersChanged);
    connect(ui->doubleSpinBox_3, &QDoubleSpinBox::valueChanged, this, &MainWindow::onSurfaceParametersChanged);
    connect(ui->doubleSpinBox_4, &QDoubleSpinBox::valueChanged, this, &MainWindow::onSurfaceParametersChanged);

    connect(ui->doubleSpinBox_5, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_6, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_7, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_8, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_9, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_10, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
    connect(ui->doubleSpinBox_14, &QDoubleSpinBox::valueChanged, this, &MainWindow::onTransformParametersChanged);
}

void MainWindow::onChooseFileClicked()
{
    const QString projectFilesPath =
        QCoreApplication::applicationDirPath() + "/../../../files_for_selecting";
    const QString filePath = QFileDialog::getOpenFileName(
        this,
        "Выберите CSV-файл",
        projectFilesPath,
        "CSV files (*.csv);;All files (*.*)");

    if (!filePath.isEmpty())
        selectFile(filePath);
}

void MainWindow::onResetButtonClicked()
{
    isChangingValues = true;

    ui->doubleSpinBox_2->setValue(0.0);
    ui->doubleSpinBox->setValue(5.0);
    ui->doubleSpinBox_3->setValue(1.0);
    ui->doubleSpinBox_4->setValue(1.0);
    ui->doubleSpinBox_5->setValue(0.0);
    ui->doubleSpinBox_6->setValue(0.0);
    ui->doubleSpinBox_7->setValue(0.0);
    ui->doubleSpinBox_8->setValue(0.0);
    ui->doubleSpinBox_9->setValue(0.0);
    ui->doubleSpinBox_10->setValue(0.0);
    ui->doubleSpinBox_14->setValue(1.0);

    isChangingValues = false;
    if (hasLoadedData() && rebuildSurface(true))
        updateTransform(true);
}

void MainWindow::onSurfaceParametersChanged()
{
    if (!isChangingValues && hasLoadedData())
        rebuildSurface(true);
}

void MainWindow::onTransformParametersChanged()
{
    if (!isChangingValues && hasLoadedData())
        updateTransform(false);
}

void MainWindow::selectFile(const QString& filePath)
{
    if (filePath.isEmpty() || !loadData(filePath))
        return;

    currentFilePath = filePath;
    ui->filePathLineEdit->setText(filePath);
    ui->contentStackedWidget->setCurrentWidget(ui->graphPage);
    updateTransform(true);
}

SurfaceParams MainWindow::surfaceParamsFromUi() const
{
    return {
        ui->doubleSpinBox_2->value(),
        ui->doubleSpinBox->value(),
        ui->doubleSpinBox_3->value(),
        ui->doubleSpinBox_4->value()
    };
}

TransformParams MainWindow::transformParamsFromUi() const
{
    return {
        {
            ui->doubleSpinBox_5->value(),
            ui->doubleSpinBox_6->value(),
            ui->doubleSpinBox_7->value()
        },
        {
            ui->doubleSpinBox_8->value(),
            ui->doubleSpinBox_9->value(),
            ui->doubleSpinBox_10->value()
        },
        ui->doubleSpinBox_14->value()
    };
}

bool MainWindow::loadData(const QString& filePath)
{
    const std::string path = filePath.toStdString();
    AppParams params = {};

    params.filePath = path.c_str();
    params.surfaceParams = surfaceParamsFromUi();
    doOperation(LOAD_DATA, &context, &params);

    return handleOperationResult(true);
}

bool MainWindow::rebuildSurface(bool fitScene)
{
    AppParams params = {};

    params.surfaceParams = surfaceParamsFromUi();
    doOperation(REBUILD_SURFACE, &context, &params);

    return handleOperationResult(fitScene);
}

bool MainWindow::updateTransform(bool fitScene)
{
    AppParams params = {};

    params.transformParams = transformParamsFromUi();
    doOperation(UPDATE_TRANSFORM, &context, &params);

    return handleOperationResult(fitScene);
}

bool MainWindow::handleOperationResult(bool fitScene)
{
    if (context.status != OK) {
        ui->statusbar->showMessage(statusText(context.status));
        return false;
    }

    sceneDrawer->drawScene(context.scene, fitScene);
    ui->statusbar->showMessage("Операция выполнена успешно");

    return true;
}

QString MainWindow::statusText(Status status) const
{
    QString text;

    switch (status) {
    case OK:
        text = "Операция выполнена успешно";
        break;
    case NULL_POINTER:
        text = "Внутренняя ошибка: получен нулевой указатель";
        break;
    case MEMORY_ERROR:
        text = "Не удалось выделить память";
        break;
    case FILE_OPEN_ERROR:
        text = "Не удалось открыть файл";
        break;
    case EMPTY_FILE:
        text = "Выбранный файл пуст";
        break;
    case INVALID_NUMBER:
        text = "CSV-файл содержит некорректное число";
        break;
    case INVALID_ROW_SIZE:
        text = "Строки CSV-файла имеют разную длину";
        break;
    case INVALID_GRID_SIZE:
        text = "Некорректный размер таблицы";
        break;
    case INVALID_NORMALIZATION_RANGE:
        text = "Минимум нормировки должен быть меньше максимума";
        break;
    case INVALID_STEP:
        text = "Шаг по осям должен быть положительным";
        break;
    case INVALID_SCALE:
        text = "Масштаб должен быть положительным";
        break;
    case DATA_NOT_LOADED:
        text = "Сначала загрузите CSV-файл";
        break;
    case UNKNOWN_OPERATION:
        text = "Неизвестная операция";
        break;
    default:
        text = "Неизвестная ошибка";
    }

    return text;
}

int MainWindow::hasLoadedData() const
{
    return context.grid.values != nullptr
           && context.grid.rows > 0
           && context.grid.columns > 0;
}
