#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME fire

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    ElementInfo *info;
    switch(getType(target)) {
        case VOID   : return 0;
        case EMPTY  : 
            moveElement(cell, target);
            return 1;
        default :
            info = &elementLookup[target->el->type];
            if(info->temp.flammable && target->el->r0 == 0) {
                target->el->r0 = (randomU8() % info->temp.burnTime) + 1;
            }
    }
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!el->rv) {
        freeCell(cell);
        spawnElement(cell, SMOKE);
        return;
    }
    el->rv -= 1;

    APPROACHIFLESS(fluid.density[cell->fluidInd], 120.0f, 0.4f);
    APPROACHIFMORE(fluid.vy[cell->fluidInd], -1.3f, 0.4f);

    attempt_fire(el, cell, getCell(x + RANDDIR, y + RANDDIR));
}

void VARPREF(init)(Element *el) {
    el->rv %= 120;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),
    .init = VARPREF(init),

    .state = s_PLASMA,
    .weight = 0,
    .baseColor = TOHSL(38, 100, 50)
};

#undef ELEMENT_NAME
