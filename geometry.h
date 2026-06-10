#ifndef GEOMETRY_H
#define GEOMETRY_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double x;
    double y;
    double z;
} Point3D;

typedef struct {
    Point3D begin;
    Point3D end;
} Edge3D;

typedef struct {
    double normalizationMin;
    double normalizationMax;
    double stepX;
    double stepY;
} SurfaceParams;

typedef struct {
    Point3D move;
    Point3D rotation;
    double scale;
} TransformParams;

#ifdef __cplusplus
}
#endif

#endif // GEOMETRY_H
