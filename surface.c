#include "surface.h"

#include <math.h>
#include <stdint.h>
#include <stdlib.h>

Status surfaceValidateInput(const Grid* grid, const SurfaceParams* params);
void surfaceFindRange(const Grid* grid, double* minimum, double* maximum);
double surfaceNormalizeValue(double value, double minimum, double maximum, const SurfaceParams* params);
void surfaceFill(Surface* surface, const Grid* grid, const SurfaceParams* params);
Status surfaceAllocate(Surface* surface, size_t rows, size_t columns);

void surfaceInit(Surface* surface)
{
    if (surface != NULL) {
        surface->points = NULL;
        surface->rows = 0;
        surface->columns = 0;
    }
}

Status surfaceBuild(Surface* surface, const Grid* grid, const SurfaceParams* params)
{
    Status status = OK;
    Surface builtSurface;

    surfaceInit(&builtSurface);
    if (surface == NULL)
        status = NULL_POINTER;
    else
        status = surfaceValidateInput(grid, params);
    if (status == OK)
        status = surfaceAllocate(&builtSurface, grid->rows, grid->columns);
    if (status == OK) {
        surfaceFill(&builtSurface, grid, params);
        surfaceFree(surface);
        *surface = builtSurface;
    } else
        surfaceFree(&builtSurface);

    return status;
}

void surfaceFree(Surface* surface)
{
    if (surface != NULL) {
        free(surface->points);
        surfaceInit(surface);
    }
}

Status surfaceValidateInput(const Grid* grid, const SurfaceParams* params)
{
    Status status = OK;

    if (grid == NULL || params == NULL)
        status = NULL_POINTER;
    else if (grid->values == NULL || grid->rows == 0 || grid->columns == 0)
        status = DATA_NOT_LOADED;
    else if (!isfinite(params->normalizationMin)
             || !isfinite(params->normalizationMax)
             || params->normalizationMin >= params->normalizationMax)
        status = INVALID_NORMALIZATION_RANGE;
    else if (!isfinite(params->stepX) || !isfinite(params->stepY)
             || params->stepX <= 0.0 || params->stepY <= 0.0)
        status = INVALID_STEP;

    return status;
}

Status surfaceAllocate(Surface* surface, size_t rows, size_t columns)
{
    Status status = OK;

    if (columns > SIZE_MAX / rows
        || rows * columns > SIZE_MAX / sizeof(Point3D))
        status = INVALID_GRID_SIZE;
    else {
        surface->points = malloc(rows * columns * sizeof(Point3D));
        if (surface->points == NULL)
            status = MEMORY_ERROR;
        else {
            surface->rows = rows;
            surface->columns = columns;
        }
    }

    return status;
}

void surfaceFindRange(const Grid* grid, double* minimum, double* maximum)
{
    size_t count = grid->rows * grid->columns;

    *minimum = grid->values[0];
    *maximum = grid->values[0];
    for (size_t index = 1; index < count; index++) {
        if (grid->values[index] < *minimum)
            *minimum = grid->values[index];
        if (grid->values[index] > *maximum)
            *maximum = grid->values[index];
    }
}

double surfaceNormalizeValue(double value, double minimum, double maximum, const SurfaceParams* params)
{
    double result = (params->normalizationMin + params->normalizationMax) / 2.0;

    if (fabs(maximum - minimum) > SURFACE_EPS)
        result = params->normalizationMin
                 + (value - minimum) / (maximum - minimum)
                       * (params->normalizationMax - params->normalizationMin);

    return result;
}

void surfaceFill(Surface* surface, const Grid* grid, const SurfaceParams* params)
{
    double minimum = 0.0;
    double maximum = 0.0;

    surfaceFindRange(grid, &minimum, &maximum);
    for (size_t row = 0; row < grid->rows; row++)
        for (size_t column = 0; column < grid->columns; column++) {
            size_t index = row * grid->columns + column;
            surface->points[index].x = column * params->stepX;
            surface->points[index].y = row * params->stepY;
            surface->points[index].z = surfaceNormalizeValue(
                grid->values[index], minimum, maximum, params);
        }
}
