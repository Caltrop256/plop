#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME cloner

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    Cell *neighbors[8];
    getMooreNeighborhood(cell, neighbors);

    if(el->r0 == 0) {
        for(U8 i = 0; i < 8; ++i) {
            ElementType type = getType(neighbors[i]);
            switch(type) {
                default :
                    el->r0 = type;
                    break;
                case VOID :
                case EMPTY :
                case CLONER :
                case UNBREAKABLECLONER :
                    break;
            }
        }
    } else {
        for(U8 i = 0; i < 8; ++i) {
            if(!neighbors[i]->el && randEveryU8(20)) spawnElement(neighbors[i], (ElementType)el->r0);
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_UNKNOWN,
    .weight = 16,
    .explosionResistance = 40,
    .baseColor = TOHSL(316, 88, 44)
};

#undef ELEMENT_NAME
