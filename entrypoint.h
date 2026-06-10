#ifndef ENTRYPOINT_H
#define ENTRYPOINT_H

#include "appcontext.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    INITIALIZE,
    LOAD_DATA,
    REBUILD_SURFACE,
    UPDATE_TRANSFORM,
    DISPOSE
} Operation;

typedef struct {
    const char* filePath;
    SurfaceParams surfaceParams;
    TransformParams transformParams;
} AppParams;

void doOperation(Operation operation, AppContext* context, const AppParams* params);

#ifdef __cplusplus
}
#endif

#endif // ENTRYPOINT_H
