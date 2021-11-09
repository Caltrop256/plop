#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME unbreakablecloner

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
            if(!neighbors[i]) continue;
            if(randEveryU8(2)) {
                if(neighbors[i]->el) {
                    if(neighbors[i]->el->type == UNBREAKABLECLONER) continue;
                    freeCell(neighbors[i]);
                }
                spawnElement(neighbors[i], (ElementType)el->r0);
            }
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .state = s_UNKNOWN,
    .weight = 255,
    .explosionResistance = 255,
    .baseColor = TOHSL(350, 88, 44)
};

#undef ELEMENT_NAME
