#include "scene.h"

#include "matrix.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

Status sceneCopySurface(Surface* destination, const Surface* source);
double sceneAxisLength(const Surface* surface);
void sceneCreateAxes(Edge3D* axes, double length);
void sceneTransformSurface(Surface* surface, const TransformMatrix* matrix);
void sceneTransformAxes(Edge3D* axes, const TransformMatrix* matrix);

void sceneInit(Scene* scene)
{
    if (scene != NULL) {
        surfaceInit(&scene->surface);
        for (int index = 0; index < SCENE_AXIS_COUNT; index++) {
            scene->axes[index].begin  = (Point3D){.x = 0, .y = 0, .z = 0};
            scene->axes[index].end = (Point3D){.x = 0, .y = 0, .z = 0};
        }
    }
}

Status sceneBuild(Scene* scene, const Surface* surface, const TransformParams* params)
{
    Status status = OK;
    Scene builtScene;
    TransformMatrix matrix;

    sceneInit(&builtScene);
    if (scene == NULL || surface == NULL || params == NULL)
        status = NULL_POINTER;
    else if (!isfinite(params->scale) || params->scale <= 0.0)
        status = INVALID_SCALE;
    if (status == OK)
        status = sceneCopySurface(&builtScene.surface, surface);
    if (status == OK) {
        matrix = matrixTransform(params);
        sceneCreateAxes(builtScene.axes, sceneAxisLength(surface));
        sceneTransformSurface(&builtScene.surface, &matrix);
        sceneTransformAxes(builtScene.axes, &matrix);
        sceneFree(scene);
        *scene = builtScene;
    } else
        sceneFree(&builtScene);

    return status;
}

void sceneFree(Scene* scene)
{
    if (scene != NULL) {
        surfaceFree(&scene->surface);
        sceneInit(scene);
    }
}

Status sceneCopySurface(Surface* destination, const Surface* source)
{
    Status status = OK;
    size_t count = 0;

    if (source->columns > SIZE_MAX / source->rows)
        status = INVALID_GRID_SIZE;
    else {
        count = source->rows * source->columns;
        if (count > SIZE_MAX / sizeof(Point3D))
            status = INVALID_GRID_SIZE;
    }
    if (status == OK) {
        destination->points = malloc(count * sizeof(Point3D));
        if (destination->points == NULL)
            status = MEMORY_ERROR;
    }
    if (status == OK) {
        destination->rows = source->rows;
        destination->columns = source->columns;
        for (size_t index = 0; index < count; index++)
            destination->points[index] = source->points[index];
    }

    return status;
}

double sceneAxisLength(const Surface* surface)
{
    double length = 0.0;
    size_t count = surface->rows * surface->columns;

    for (size_t index = 0; index < count; index++) {
        if (fabs(surface->points[index].x) > length)
            length = fabs(surface->points[index].x);
        if (fabs(surface->points[index].y) > length)
            length = fabs(surface->points[index].y);
        if (fabs(surface->points[index].z) > length)
            length = fabs(surface->points[index].z);
    }

    return length + 1.0;
}

void sceneCreateAxes(Edge3D* axes, double length)
{
    axes[0].begin = (Point3D){-length, 0.0, 0.0};
    axes[0].end = (Point3D){length, 0.0, 0.0};
    axes[1].begin = (Point3D){0.0, -length, 0.0};
    axes[1].end = (Point3D){0.0, length, 0.0};
    axes[2].begin = (Point3D){0.0, 0.0, -length};
    axes[2].end = (Point3D){0.0, 0.0, length};
}

void sceneTransformSurface(Surface* surface, const TransformMatrix* matrix)
{
    size_t count = surface->rows * surface->columns;

    for (size_t index = 0; index < count; index++)
        surface->points[index] = matrixTransformPoint(matrix, &surface->points[index]);
}

void sceneTransformAxes(Edge3D* axes, const TransformMatrix* matrix)
{
    for (int index = 0; index < SCENE_AXIS_COUNT; index++) {
        axes[index].begin = matrixTransformPoint(matrix, &axes[index].begin);
        axes[index].end = matrixTransformPoint(matrix, &axes[index].end);
    }
}
