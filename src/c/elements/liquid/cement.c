#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME cement

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) {
        ElementInfo *info = &elementLookup[target->el->type];
        switch(info->state) {
            case s_LIQUID :
                if(info->liquid.density <= elementLookup[el->type].liquid.density) {
                    if(CEMENT == target->el->type && randEveryU8(50)) el->color += 1;
                    return 0;
                }
            case s_GAS :
                    swapElements(cell, target);
                    return 1;
            case s_PLASMA :
                freeCell(target);
                moveElement(cell, target);
                return 1;
        }
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!attempt_cement(el, cell, getCell(x, y + 1))) {
        I8 dir = LORDIR;
        if(!attempt_cement(el, cell, getCell(x + dir, y)))
            if(!attempt_cement(el, cell, getCell(x - dir, y))) {
                el->rv -= 1;
                if(el->rv == 0) {
                    freeCell(cell);
                    spawnElement(cell, STONE);
                }
                return;
            }
    }
}

void VARPREF(init)(Element *el) {
    el->rv = 200 + randomU8() % 55;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),
    .init = VARPREF(init),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 1,
        .density = 3
    },

    .weight = 4.6f,
    .baseColor = TOHSL(0, 0, 79)
};

#undef ELEMENT_NAME
