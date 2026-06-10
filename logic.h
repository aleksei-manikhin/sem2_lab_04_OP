#ifndef LOGIC_H
#define LOGIC_H

#include "appcontext.h"

#ifdef __cplusplus
extern "C" {
#endif

void initContext(AppContext* context);
Status loadData(AppContext* context, const char* filePath, const SurfaceParams* params);//Новый csv
Status rebuildSurface(AppContext* context, const SurfaceParams* params); //Нормировка + шаг
Status updateTransform(AppContext* context, const TransformParams* params);// Масштаб + поворот + сдвиг
void disposeContext(AppContext* context);

#ifdef __cplusplus
}
#endif

#endif // LOGIC_H
