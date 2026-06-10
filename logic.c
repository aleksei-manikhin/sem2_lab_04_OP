#include "logic.h"

#include "csv_reader.h"

SurfaceParams logicDefaultSurfaceParams(void);
TransformParams logicDefaultTransformParams(void);
Status logicBuildModel(const Grid* grid, const SurfaceParams* surfaceParams,
                       const TransformParams* transformParams, Surface* surface, Scene* scene);
void logicReplaceData(AppContext* context, Grid* grid, Surface* surface,
                      Scene* scene, const SurfaceParams* params);
void logicReplaceSurface(AppContext* context, Surface* surface, Scene* scene,
                         const SurfaceParams* params);
Status logicLoadModel(const char* filePath, const SurfaceParams* params,
                      const TransformParams* transformParams, Grid* grid, Surface* surface, Scene* scene);
void logicFreeModel(Grid* grid, Surface* surface, Scene* scene);

void initContext(AppContext* context)
{
    if (context != NULL) {
        gridInit(&context->grid);
        surfaceInit(&context->surface);
        sceneInit(&context->scene);
        context->surfaceParams = logicDefaultSurfaceParams();
        context->transformParams = logicDefaultTransformParams();
        context->status = OK;
    }
}

Status loadData(AppContext* context, const char* filePath, const SurfaceParams* params)
{
    Status status = OK;
    Grid loadedGrid;
    Surface builtSurface;
    Scene builtScene;
    TransformParams transformParams = logicDefaultTransformParams();
    gridInit(&loadedGrid);
    surfaceInit(&builtSurface);
    sceneInit(&builtScene);
    if (context == NULL || filePath == NULL || params == NULL)
        status = NULL_POINTER;
    else
        status = logicLoadModel(filePath, params, &transformParams,
                                &loadedGrid, &builtSurface, &builtScene);
    if (status == OK) {
        logicReplaceData(context, &loadedGrid, &builtSurface, &builtScene, params);
        context->transformParams = transformParams;
    } else
        logicFreeModel(&loadedGrid, &builtSurface, &builtScene);
    if (context != NULL)
        context->status = status;

    return status;
}

Status rebuildSurface(AppContext* context, const SurfaceParams* params)
{
    Status status = OK;
    Surface rebuiltSurface;
    Scene rebuiltScene;

    surfaceInit(&rebuiltSurface);
    sceneInit(&rebuiltScene);
    if (context == NULL || params == NULL)
        status = NULL_POINTER;
    else
        status = logicBuildModel(&context->grid, params, &context->transformParams,
                                 &rebuiltSurface, &rebuiltScene);
    if (status == OK)
        logicReplaceSurface(context, &rebuiltSurface, &rebuiltScene, params);
    else {
        surfaceFree(&rebuiltSurface);
        sceneFree(&rebuiltScene);
    }
    if (context != NULL)
        context->status = status;

    return status;
}

Status updateTransform(AppContext* context, const TransformParams* params)
{
    Status status = OK;
    Scene rebuiltScene;

    sceneInit(&rebuiltScene);
    if (context == NULL || params == NULL)
        status = NULL_POINTER;
    else
        status = sceneBuild(&rebuiltScene, &context->surface, params);
    if (status == OK) {
        sceneFree(&context->scene);
        context->scene = rebuiltScene;
        context->transformParams = *params;
    } else
        sceneFree(&rebuiltScene);
    if (context != NULL)
        context->status = status;

    return status;
}

void disposeContext(AppContext* context)
{
    if (context != NULL) {
        gridFree(&context->grid);
        surfaceFree(&context->surface);
        sceneFree(&context->scene);
        context->surfaceParams = logicDefaultSurfaceParams();
        context->transformParams = logicDefaultTransformParams();
        context->status = OK;
    }
}

SurfaceParams logicDefaultSurfaceParams(void)
{
    SurfaceParams params = {0.0, 5.0, 1.0, 1.0};

    return params;
}

TransformParams logicDefaultTransformParams(void)
{
    TransformParams params = {
        {0.0, 0.0, 0.0},
        {0.0, 0.0, 0.0},
        1.0
    };

    return params;
}

Status logicBuildModel(const Grid* grid, const SurfaceParams* surfaceParams,
                       const TransformParams* transformParams, Surface* surface, Scene* scene)
{
    Status status = surfaceBuild(surface, grid, surfaceParams);

    if (status == OK)
        status = sceneBuild(scene, surface, transformParams);

    return status;
}

Status logicLoadModel(const char* filePath, const SurfaceParams* params,
                      const TransformParams* transformParams,
                      Grid* grid, Surface* surface, Scene* scene)
{
    Status status = csvReadGrid(filePath, grid);

    if (status == OK)
        status = logicBuildModel(grid, params, transformParams, surface, scene);

    return status;
}

void logicFreeModel(Grid* grid, Surface* surface, Scene* scene)
{
    gridFree(grid);
    surfaceFree(surface);
    sceneFree(scene);
}

void logicReplaceData(AppContext* context, Grid* grid, Surface* surface,
                      Scene* scene, const SurfaceParams* params)
{
    gridFree(&context->grid);
    surfaceFree(&context->surface);
    sceneFree(&context->scene);
    context->grid = *grid;
    context->surface = *surface;
    context->scene = *scene;
    context->surfaceParams = *params;
}

void logicReplaceSurface(AppContext* context, Surface* surface, Scene* scene,
                         const SurfaceParams* params)
{
    surfaceFree(&context->surface);
    sceneFree(&context->scene);
    context->surface = *surface;
    context->scene = *scene;
    context->surfaceParams = *params;
}
