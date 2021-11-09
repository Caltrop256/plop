#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME nitro

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
                } else if(NITRO == target->el->type && randEveryU8(15)) {
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
    F32 fvx = fluid->vx[cell->fluidInd];
    F32 fvy = fluid->vy[cell->fluidInd];
    if(el->r0 || fvx * fvx + fvy * fvy >= 10 * 10) {
        freeCell(cell);
        explode(x, y, el->rv % 40);
        return;
    }
    update_liquid(cell);
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 2,
        .density = 2
    },

    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 3
    },

    .weight = 3,
    .baseColor = TOHSL(115, 98, 47)
};

#undef ELEMENT_NAME
