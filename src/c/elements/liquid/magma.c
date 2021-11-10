#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME magma

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) {
        ElementInfo *info = &elementLookup[target->el->type];
        if(info->temp.flammable && target->el->r0 == 0) {
            target->el->r0 = (randomU8() % info->temp.burnTime) + 1;
        }

        switch(info->state) {
            case s_GAS :
                swapElements(cell, target);
                return 1;
            case s_LIQUID :
                if(info->liquid.density > elementLookup[el->type].liquid.density) {
                    if(randEveryU8(3)) {
                        swapElements(cell, target);
                        return 1;
                    }
                } else if(MAGMA == target->el->type && randEveryU8(100)) {
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
    if(el->scorched) el->scorched = 0;
    if(cell->temperature < 950.0f) {
        el->rv -= 1;
        if(!el->rv) {
            freeCell(cell);
            spawnElement(cell, COBBLESTONE);
            return;
        }
    }
    APPROACHIFLESS(fluid.density[cell->fluidInd], 1000.0f, 0.95);
    
    if(randomBool()) update_liquid(cell);
    if(randEveryU8(40)) {
        Cell *target = getCell(x + RANDDIR, y + RANDDIR);
        if(getType(target) == EMPTY) {
            spawnElement(target, FIRE);
            target->el->rv %= 30;
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 1,
        .density = 0
    },

    .weight = 9,
    .baseColor = TOHSL(8, 100, 50)
};

#undef ELEMENT_NAME
