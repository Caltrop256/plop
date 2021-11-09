#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME battery

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    Cell *neighbors[8];
    getMooreNeighborhood(cell, neighbors);
    for(U8 i = 0; i < 8; ++i) {
        Cell *target = neighbors[i];
        if(
            target && 
            target->el && 
            !target->el->electricityState && 
            target->el->type != WATER &&
            elementLookup[target->el->type].conductive
        ) {
            target->el->electricityState += 1;
            fluid->density[cell->fluidInd] += 2;
        }
    }

    if(cell->temperature >= 260.0f) {
        freeCell(cell);
        explode(x, y, 60);
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_SOLID,
    .weight = 255,
    .isStatic = 1,
    .explosionResistance = 12,
    .baseColor = TOHSL(62, 77, 25)
};

#undef ELEMENT_NAME
