#include "util.h"

extern orxOBJECT* sol_util_make_object(orxOBJECT *win, const char *name) {
    if (name == NULL) { return NULL; }
    orxVECTOR pos;
    orxOBJECT *obj = orxObject_CreateFromConfig(name);

    if (!obj) { return NULL; }

    orxObject_SetParent(obj, win);
    orxObject_GetPosition(obj, &pos);
    pos.fX = 100;
    pos.fY = 100;
    pos.fZ -= 0.01;
    orxObject_SetPosition(obj, &pos);

    return obj;
}
