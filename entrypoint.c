#include "entrypoint.h"

#include "logic.h"

void doOperation(Operation operation, AppContext* context, const AppParams* params)
{
    switch (operation) {
    case INITIALIZE:
        initContext(context);
        break;
    case LOAD_DATA:
        loadData(context, params->filePath, &params->surfaceParams);
        break;
    case REBUILD_SURFACE:
        rebuildSurface(context, &params->surfaceParams);
        break;
    case UPDATE_TRANSFORM:
        updateTransform(context, &params->transformParams);
        break;
    case DISPOSE:
        disposeContext(context);
        break;
    default:
        context->status = UNKNOWN_OPERATION;
        break;
    }
}
