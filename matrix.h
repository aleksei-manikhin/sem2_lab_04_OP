#ifndef MATRIX_H
#define MATRIX_H

#include "geometry.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    MATRIX_SIZE = 4,
    COORDINATE_COUNT = 3
};

typedef struct {
    double values[MATRIX_SIZE][MATRIX_SIZE];
} TransformMatrix;

TransformMatrix matrixIdentity(void);
TransformMatrix matrixMultiply(const TransformMatrix* first, const TransformMatrix* second);
TransformMatrix matrixMove(const Point3D* move);
TransformMatrix matrixScale(double scale);
TransformMatrix matrixRotateX(double angle);
TransformMatrix matrixRotateY(double angle);
TransformMatrix matrixRotateZ(double angle);
TransformMatrix matrixRotate(const Point3D* rotation);
TransformMatrix matrixTransform(const TransformParams* params);
Point3D matrixTransformPoint(const TransformMatrix* matrix, const Point3D* point);

#ifdef __cplusplus
}
#endif

#endif // MATRIX_H
