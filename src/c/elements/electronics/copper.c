#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME copper

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->electricityState == 1) {
        APPROACHIFLESS(fluid->density[cell->fluidInd], 600.0f, 0.999);
    }

    if(cell->temperature >= 500.0f) {
        freeCell(cell);
        spawnElement(cell, MAGMA);
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .weight = 60,
    .isStatic = 1,
    .conductive = 1,
    .explosionResistance = 16,
    .baseColor = TOHSL(25, 74, 16)
};

#undef ELEMENT_NAME
