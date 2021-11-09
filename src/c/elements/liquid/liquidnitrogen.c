#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME liquidnitrogen

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) {
        if(getType(target) != FOG && ABS(target->temperature - cell->temperature) > 10) {
            freeCell(cell);
            return 0;
        }
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
                } else if(LIQUIDNITROGEN == target->el->type && randEveryU8(10)) {
                    el->color += 1;
                }
                break;
            case s_SOLID :
                return 0;
            case s_PLASMA :
                freeCell(target);
                moveElement(cell, target);
                return 1;
        }
    };
    return 0;
}
void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
        ElementInfo *self = &elementLookup[el->type];

        APPROACHIFMORE(fluid->density[cell->fluidInd], -195.8f, 0.99);

        if(randEveryU8(4)) {
            Cell *target = getCell(x + RANDDIR, y + RANDDIR);
            if(getType(target) == EMPTY) spawnElement(target, FOG);
        }

        if(!attempt_liquidnitrogen(el, cell, getCell(x, y + 1))) {
            if(cell->el && cell->el->type == LIQUIDNITROGEN) moveHorizontally(cell, (randomU8() % self->liquid.dispersion + 1) * LORDIR, self->attempt);
        };
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 4,
        .density = 4
    },

    .weight = 1,
    .baseColor = TOHSL(190, 100, 70)
    
};

#undef ELEMENT_NAME
