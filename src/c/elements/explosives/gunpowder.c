#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME gunpowder

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    ElementType targetType = getType(target);
    ElementInfo *targetInfo = &elementLookup[targetType];

    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) switch(targetInfo->state) {
        case s_LIQUID : if(!randEveryU8(4)) return 0;
        case s_GAS :
        case s_PLASMA :
            swapElements(cell, target);
            return 1;
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0) {
        freeCell(cell);
        explode(cell->x, cell->y, el->rv % 20);
        return;
    }
    if(!attempt_gunpowder(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(attempt_gunpowder(el, cell, getCell(x + dir, y + 1)));
            else if(attempt_gunpowder(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 3,
        .impactScatter = 3.0f
    },
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 1
    },
    .weight = 4,
    .explosionResistance = 16,
    .baseColor = TOHSL(26, 91, 54)
};

#undef ELEMENT_NAME
