#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"
#include "../subatomics.h"

#define ELEMENT_NAME firework

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
    if(el->electricityState) el->r0 = 1;
    if(el->r0) {
        el->rv -= 1;
        if(el->rv == 0) {
            freeCell(cell);
            explode(cell->x, cell->y, randomU8() % 15);

            /* 
            PHOTONS have a chance of spawning into freed up element memory, 
            which might make the main cell-tick function think the element is still around,
            which can create havoc

            U8 i = randomU8() % 64;
            while(i --> 0) createSubatomicHelper(x, y, randomU8() % 7, randomU8());*/
            return;
        }
        el->sbpy = -elementLookup[FIREWORK].weight - 1.5;
        if(attempt_firework(el, cell, getCell(x, y - 1))) {
            if(getType(cell) == EMPTY && randEveryU8(4)) spawnElement(cell, SMOKE);
        }
    } else {
        if(!attempt_firework(el, cell, getCell(x, y + 1))) {
            if(!el->halted) {
                int8_t dir = LORDIR;
                if(attempt_firework(el, cell, getCell(x + dir, y + 1)));
                else if(attempt_firework(el, cell, getCell(x - dir, y + 1)));
                else attemptHalt(el);
            } else tryReleaseHalted(cell);
        } else el->halted = 0;
    }
}

void VARPREF(init)(Element *el) {
    el->rv = randomU8() % 80 + 30;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),
    .init = VARPREF(init),

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
    .conductive = 1,
    .explosionResistance = 16,
    .baseColor = TOHSL(0, 100, 50)
};

#undef ELEMENT_NAME
