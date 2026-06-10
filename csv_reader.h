#ifndef CSV_READER_H
#define CSV_READER_H

#include "grid.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    CSV_INITIAL_LINE_CAPACITY = 128,
};

Status csvReadGrid(const char* filePath, Grid* grid);

#ifdef __cplusplus
}
#endif

#endif // CSV_READER_H
