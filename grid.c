#include "grid.h"

#include <stdint.h>
#include <stdlib.h>

void gridInit(Grid* grid)
{
    if (grid != NULL) {
        grid->values = NULL;
        grid->rows = 0;
        grid->columns = 0;
    }
}

Status gridAllocate(Grid* grid, size_t rows, size_t columns)
{
    Status status = OK;
    double* values = NULL;

    if (grid == NULL)
        status = NULL_POINTER;
    else if (rows == 0 || columns == 0)
        status = INVALID_GRID_SIZE;
    else if (columns > SIZE_MAX / rows)
        status = INVALID_GRID_SIZE;
    else if (rows * columns > SIZE_MAX / sizeof(double))
        status = INVALID_GRID_SIZE;
    else {
        values = calloc(rows * columns, sizeof(double));
        if (values == NULL)
            status = MEMORY_ERROR;
        else {
            free(grid->values);
            grid->values = values;
            grid->rows = rows;
            grid->columns = columns;
        }
    }

    return status;
}

void gridFree(Grid* grid)
{
    if (grid != NULL) {
        free(grid->values);
        gridInit(grid);
    }
}
