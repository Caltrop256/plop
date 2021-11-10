#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME thermite

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0) {
        if(el->r0 == 1) {
            freeCell(cell);
            return;
        }

        APPROACHIFMORE(fluid.vy[cell->fluidInd], -10.0f, 0.6);

        if(randEveryU8(255)) {
            freeCell(cell);
            explode(cell->x, cell->y, el->rv % 20);
            return;
        }

        Cell *n[8];
        getMooreNeighborhood(cell, n);
        for(U8 i = 0; i < 8; ++i) {
            if(n[i]) {
                if(!n[i]->el) {
                    if(randEveryU8(4)) spawnElement(n[i], FIRE);
                    else {
                        spawnElement(n[i], DEBRIS);
                        n[i]->el->r0 = MAGMA;
                        n[i]->el->rv = randomU8();
                    }
                }
            }
        }
    }
    if(!default_attempt(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(default_attempt(el, cell, getCell(x + dir, y + 1)));
            else if(default_attempt(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 3,
        .impactScatter = 3.0f
    },
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 3
    },
    .weight = 4,
    .explosionResistance = 16,
    .baseColor = TOHSL(45, 61, 69)
};

#undef ELEMENT_NAME
