#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME fuse

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0) {
        if(el->r0 == 6) {
            Cell *neighbors[8];
            getMooreNeighborhood(cell, neighbors);
            for(U8 i = 0; i < 8; ++i) {
                if(neighbors[i]) {
                    if(neighbors[i]->el) {
                        if(neighbors[i]->el->type == FUSE && !neighbors[i]->el->r0) {
                            neighbors[i]->el->r0 = 1;
                        } else if(neighbors[i]->el->electricityState == 0) neighbors[i]->el->electricityState = 1;
                    } else if(randEveryU8(4)) {
                        spawnElement(neighbors[i], FIRE);
                        neighbors[i]->el->rv %= 10;
                    }
                }
            }
        }
        el->r0 += 1;

        if(el->r0 == 12) {
            freeCell(cell);
            return;
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 1,
        .burnTemperature = 35,
    },

    .state = s_SOLID,
    .weight = 14,
    .isStatic = 1,
    .explosionResistance = 9,
    .baseColor = TOHSL(55, 32, 67)
};

#undef VARIABLE
