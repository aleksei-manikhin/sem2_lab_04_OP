#ifndef SURFACE_H
#define SURFACE_H

#include "geometry.h"
#include "grid.h"

#define SURFACE_EPS 1e-9

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Point3D* points;
    size_t rows;
    size_t columns;
} Surface;

void surfaceInit(Surface* surface);
Status surfaceBuild(Surface* surface, const Grid* grid, const SurfaceParams* params);
void surfaceFree(Surface* surface);

#ifdef __cplusplus
}
#endif

#endif // SURFACE_H
