#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME acid

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    switch(getType(target)) {
        case ACID : if(randEveryU8(10)) el->color += 1;
        case UNBREAKABLECLONER :
        case TITANIUM :
        case VOID : break;
        case EMPTY : moveElement(cell, target); return 1;
        case TOXIC : swapElements(cell, target); return 1;
        default :
            freeCell(target);
            freeCell(cell);

            if(randEveryU8(4)) spawnElement(cell, TOXIC);
            break;
    }
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!attempt_acid(el, cell, getCell(x, y + 1))) {
        if(cell->el->type == ACID) moveHorizontally(cell, (randomU8() % elementLookup[ACID].liquid.dispersion + 1) * LORDIR, attempt_acid);
    };
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_LIQUID,
    .liquid = (LiquidInfo){
        .dispersion = 3,
        .density = 0xff
    },

    .weight = 3.1f,
    .baseColor = TOHSL(112, 93, 53)
};

#undef ELEMENT_NAME
