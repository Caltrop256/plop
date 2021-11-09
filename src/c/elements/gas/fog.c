#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME fog

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    }
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    el->rv -= 1;
    if(el->rv == 0) {
        freeCell(cell);
        return;
    }

    attempt_fog(el, cell, getCell(x + RANDDIR, y + randEveryU8(10)));
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_GAS,
    .weight = 0,
    .baseColor = TOHSL(0, 0, 98)
};

#undef ELEMENT_NAME