#ifndef APP_CONTEXT_H
#define APP_CONTEXT_H

#include "grid.h"
#include "scene.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    Grid grid;
    Surface surface;
    Scene scene;
    SurfaceParams surfaceParams;
    TransformParams transformParams;
    Status status;
} AppContext;

#ifdef __cplusplus
}
#endif

#endif // APP_CONTEXT_H
