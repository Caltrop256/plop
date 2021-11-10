#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME oil

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) {
        ElementInfo *info = &elementLookup[target->el->type];
        switch(info->state) {
            case s_GAS :
                if(randEveryU8(3)) {
                    swapElements(cell, target);
                    return 1;
                }
                break;
            case s_LIQUID :
                if(info->liquid.density > elementLookup[el->type].liquid.density) {
                    if(randEveryU8(3)) {
                        swapElements(cell, target);
                        return 1;
                    }
                } else if(OIL == target->el->type && randEveryU8(15)) {
                    el->color += 1;
                }
                break;
            case s_PLASMA :
                freeCell(target);
                moveElement(cell, target);
                return 1;
        }
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    update_liquid(cell);
    cell = el->cell;
    update_flammable(cell);
    if(cell->el && cell->el->r0) {
        Cell *n[8];
        getMooreNeighborhood(cell, n);
        for(U8 i = 0; i < 8; ++i) {
            if(n[i] && !n[i]->el && randEveryU8(6)) {
                spawnElement(n[i], SMOKE);
                n[i]->el->rv = 0xff;
                fluid.vy[n[i]->fluidInd] -= 1;
            }
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 2,
        .density = 5
    },

    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 3,
        .burnTemperature = 300,
    },

    .weight = 3,
    .baseColor = TOHSL(38, 100, 18)
};

#undef ELEMENT_NAME
