#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME cobblestone

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    ElementType targetType = getType(target);
    ElementInfo *targetInfo = &elementLookup[targetType];

    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) switch(targetInfo->state) {
        case s_LIQUID : if(targetType == MAGMA || !randEveryU8(4)) return 0;
        case s_GAS :
        case s_PLASMA :
            swapElements(cell, target);
            return 1;
        case s_SOLID :
            if((targetType == PLUTONIUM || targetType == POLONIUM) && randEveryU8(3)) {
                swapElements(cell, target);
                return 1;
            }
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(cell->temperature >= 900.0f) {
        freeCell(cell);
        spawnElement(cell, MAGMA);
        return;
    }
    if(!attempt_cobblestone(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(attempt_cobblestone(el, cell, getCell(x + dir, y + 1)));
            else if(attempt_cobblestone(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 15,
        .impactScatter = 0.0f
    },
    .temp = (TemperatureInfo){
        .insolation = 0.1f
    },
    .weight = 7,
    .explosionResistance = 2,
    .baseColor = TOHSL(0, 0, 19)
};

#undef ELEMENT_NAME
