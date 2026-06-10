#ifndef GRID_H
#define GRID_H

#include "status.h"

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    double* values;
    size_t rows;
    size_t columns;
} Grid;

void gridInit(Grid* grid);
Status gridAllocate(Grid* grid, size_t rows, size_t columns);
void gridFree(Grid* grid);

#ifdef __cplusplus
}
#endif

#endif // GRID_H
