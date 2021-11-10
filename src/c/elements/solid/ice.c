#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME ice

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return default_attempt(el, cell, target);
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    APPROACHIFMORE(fluid.density[cell->fluidInd], -5.1f, 0.98);
    if(cell->temperature >= 0.6f) {
        freeCell(cell);
        spawnElement(cell, WATER);
        return;
    }
}
// TODO: add init function to set temperature

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .weight = 4,
    .isStatic = 1,
    .explosionResistance = 30,
    .baseColor = TOHSL(206, 40, 63)
};

#undef ELEMENT_NAME
