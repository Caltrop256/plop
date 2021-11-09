#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME water

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
                } else if(WATER == target->el->type && randEveryU8(10)) {
                    el->color += 1;
                }
                break;
            case s_SOLID :
                if(target->el->type == SNOW) {
                    if(cell->temperature < -2.0f) {
                        freeCell(cell);
                        spawnElement(cell, ICE);
                    } else if(randEveryU8(10)) {
                        freeCell(target);
                        spawnElement(target, WATER);
                    }
                } else if(target->el->type == ICE && cell->temperature < -1.3f) {
                    freeCell(cell);
                    spawnElement(cell, ICE);
                    return 1;
                }
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
    if(cell->temperature >= 100.0f) {
        freeCell(cell);
        spawnElement(cell, STEAM);
        return;
    } else if(cell->temperature < -0.3f && el->rv >= 127) {
        freeCell(cell);
        spawnElement(cell, SNOW);
    } else if(cell->temperature < -5.0f) {
        freeCell(cell);
        spawnElement(cell, randEveryU8(3) ? SNOW : ICE);
    } else {
        ElementInfo *self = &elementLookup[el->type];

        APPROACHIFMORE(fluid->density[cell->fluidInd], 5.5f, 0.99);

        if(!attempt_water(el, cell, getCell(x, y + 1))) {
            if(cell->el->type == WATER) moveHorizontally(cell, (randomU8() % self->liquid.dispersion + 1) * LORDIR, self->attempt);
        };
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

    .conductive = 1,

    .weight = 1,
    .baseColor = TOHSL(190, 100, 50)
    
};

#undef ELEMENT_NAME
