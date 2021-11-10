#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME pellet

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!el->r0) {
        el->rv -= 1;
        F32 fvx = fluid.vx[cell->fluidInd];
        F32 fvy = fluid.vy[cell->fluidInd];
        if((el->rv == 0 && randEveryU8(14)) || (fvx * fvx + fvy * fvy) >= 6 * 6) {
            freeCell(cell);
            spawnElement(cell, SEED);
            cell->el->sbpx = randomF32() * LORDIR * 15;
            cell->el->sbpy = randomF32() * -1 * 15;
            return;
        }
    } else {
        update_flammable(cell);
    }
}

void VARPREF(init)(Element *el) {
    el->color = g_tick;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .init = VARPREF(init),

    .state = s_SOLID,
    .isStatic = 1,
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 2,
        .burnTemperature = 30
    },
    .weight = 2.3,
    .explosionResistance = 1,
    .baseColor = TOHSL(0, 100, 50)
};

#undef ELEMENT_NAME
