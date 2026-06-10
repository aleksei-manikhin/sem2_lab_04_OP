#ifndef STATUS_H
#define STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    OK,
    NULL_POINTER,
    MEMORY_ERROR,
    FILE_OPEN_ERROR,
    EMPTY_FILE,
    INVALID_NUMBER,
    INVALID_ROW_SIZE,
    INVALID_GRID_SIZE,
    INVALID_NORMALIZATION_RANGE,
    INVALID_STEP,
    INVALID_SCALE,
    DATA_NOT_LOADED,
    UNKNOWN_OPERATION
} Status;

#ifdef __cplusplus
}
#endif

#endif // STATUS_H
