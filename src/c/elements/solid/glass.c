#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME glass

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    }
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    el->scorched = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .weight = 14,
    .isStatic = 1,
    .explosionResistance = 3,
    .colors = {
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf),
        COLOR(0xaec6cf)
    }
};

#undef ELEMENT_NAME
