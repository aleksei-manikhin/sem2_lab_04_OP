#include "matrix.h"

#include <math.h>

double degreesToRadians(double angle);

TransformMatrix matrixIdentity(void)
{
    TransformMatrix identityMatrix = {0};

    for (int index = 0; index < MATRIX_SIZE; index++)
        identityMatrix.values[index][index] = 1.0;

    return identityMatrix;
}

TransformMatrix matrixMultiply(const TransformMatrix* first, const TransformMatrix* second)
{
    TransformMatrix result = {0};

    for (int row = 0; row < MATRIX_SIZE; row++)
        for (int column = 0; column < MATRIX_SIZE; column++)
            for (int index = 0; index < MATRIX_SIZE; index++)
                result.values[row][column] +=
                    first->values[row][index] * second->values[index][column];

    return result;
}

TransformMatrix matrixMove(const Point3D* move)
{
    TransformMatrix matrix = matrixIdentity();

    matrix.values[0][3] = move->x;
    matrix.values[1][3] = move->y;
    matrix.values[2][3] = move->z;

    return matrix;
}

TransformMatrix matrixScale(double scale)
{
    TransformMatrix matrix = matrixIdentity();

    for (int index = 0; index < COORDINATE_COUNT; index++)
        matrix.values[index][index] = scale;

    return matrix;
}

TransformMatrix matrixRotateX(double angle)
{
    TransformMatrix matrix = matrixIdentity();
    double radians = degreesToRadians(angle);
    double sinAngle = sin(radians);
    double cosAngle = cos(radians);

    matrix.values[1][1] = cosAngle;
    matrix.values[1][2] = -sinAngle;
    matrix.values[2][1] = sinAngle;
    matrix.values[2][2] = cosAngle;

    return matrix;
}

TransformMatrix matrixRotateY(double angle)
{
    TransformMatrix matrix = matrixIdentity();
    double radians = degreesToRadians(angle);
    double sinAngle = sin(radians);
    double cosAngle = cos(radians);

    matrix.values[0][0] = cosAngle;
    matrix.values[0][2] = sinAngle;
    matrix.values[2][0] = -sinAngle;
    matrix.values[2][2] = cosAngle;

    return matrix;
}

TransformMatrix matrixRotateZ(double angle)
{
    TransformMatrix matrix = matrixIdentity();
    double radians = degreesToRadians(angle);
    double sinAngle = sin(radians);
    double cosAngle = cos(radians);

    matrix.values[0][0] = cosAngle;
    matrix.values[0][1] = -sinAngle;
    matrix.values[1][0] = sinAngle;
    matrix.values[1][1] = cosAngle;

    return matrix;
}

TransformMatrix matrixRotate(const Point3D* rotation)
{
    TransformMatrix xMatrix = matrixRotateX(rotation->x);
    TransformMatrix yMatrix = matrixRotateY(rotation->y);
    TransformMatrix zMatrix = matrixRotateZ(rotation->z);
    TransformMatrix xyMatrix = matrixMultiply(&yMatrix, &xMatrix);

    return matrixMultiply(&zMatrix, &xyMatrix);
}

TransformMatrix matrixTransform(const TransformParams* params)
{
    TransformMatrix moveMatrix = matrixMove(&params->move);
    TransformMatrix rotateMatrix = matrixRotate(&params->rotation);
    TransformMatrix scaleMatrix = matrixScale(params->scale);
    TransformMatrix rotateScale = matrixMultiply(&rotateMatrix, &scaleMatrix);

    return matrixMultiply(&moveMatrix, &rotateScale);
}

Point3D matrixTransformPoint(const TransformMatrix* matrix, const Point3D* point)
{
    Point3D result = {0};
    double source[MATRIX_SIZE] = {point->x, point->y, point->z, 1.0};
    double transformed[MATRIX_SIZE] = {0};

    for (int row = 0; row < MATRIX_SIZE; row++)
        for (int column = 0; column < MATRIX_SIZE; column++)
            transformed[row] += matrix->values[row][column] * source[column];

    result.x = transformed[0];
    result.y = transformed[1];
    result.z = transformed[2];

    return result;
}

double degreesToRadians(double angle)
{
    return angle * acos(-1.0) / 180.0;
}
