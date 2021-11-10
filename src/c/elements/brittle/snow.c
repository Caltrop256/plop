#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME snow

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    ElementType targetType = getType(target);
    ElementInfo *targetInfo = &elementLookup[targetType];

    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) switch(targetInfo->state) {
        case s_LIQUID : 
            if(randEveryU8(12)) {
                freeCell(cell);
                spawnElement(cell, WATER);
            }
            return 0;
        case s_GAS :
        case s_PLASMA :
            swapElements(cell, target);
            return 1;
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    APPROACHIFMORE(fluid.density[cell->fluidInd], -2.3f, 0.99);
    if(cell->temperature >= 3.6f) {
        freeCell(cell);
        spawnElement(cell, WATER);
        return;
    }

    if(!attempt_snow(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(attempt_snow(el, cell, getCell(x + dir, y + 1)));
            else if(attempt_snow(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 2,
        .impactScatter = 0.49f
    },
    .weight = 1.6f,
    .explosionResistance = 1,
    .baseColor = TOHSL(0, 0, 100)
};

#undef ELEMENT_NAME
