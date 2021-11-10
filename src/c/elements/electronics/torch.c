#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME torch

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->electricityState == 1) {
        APPROACHIFLESS(fluid.density[cell->fluidInd], 600.0f, 0.999);
        Cell *target = getCell(x, y - 1);
        if(getType(target) == EMPTY) spawnElement(target, FIRE);
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .weight = 60,
    .isStatic = 1,
    .conductive = 1,
    .explosionResistance = 7,
    .baseColor = TOHSL(54, 13, 44)
};

#undef ELEMENT_NAME
