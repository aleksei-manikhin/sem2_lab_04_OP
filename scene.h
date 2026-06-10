#ifndef SCENE_H
#define SCENE_H

#include "surface.h"

#ifdef __cplusplus
extern "C" {
#endif

enum {
    SCENE_AXIS_COUNT = 3
};

typedef struct {
    Surface surface;
    Edge3D axes[SCENE_AXIS_COUNT];
} Scene;

void sceneInit(Scene* scene);
Status sceneBuild(Scene* scene, const Surface* surface, const TransformParams* params);
void sceneFree(Scene* scene);

#ifdef __cplusplus
}
#endif

#endif // SCENE_H
