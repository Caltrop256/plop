#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME plant

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(cell->temperature <= 3.4f || el->sbpx * el->sbpx + el->sbpy * el->sbpy >= 10 * 10) {
        freeCell(cell);
        spawnElement(cell, DIRT);
        return;
    };
    if(el->r0 == 1) {
        freeCell(cell);
        return;
    } else if(el->r0 > 1) {
        el->r0 -= 1;
        Cell *target = getCell(x + RANDDIR, y + RANDDIR);
        if(getType(target) == EMPTY) spawnElement(target, FIRE);
    }

    if(el->rv > 240) {
        Cell *target = getCell(x, y + 1);
        if(getType(target) == EMPTY) {
            spawnElement(target, PLANT);
            target->el->rv = el->rv - 1;
        }
        el->rv = 0;
    }

    if(el->rv > 10 && el->rv < 40) {
        Cell *neighbors[8];
        getMooreNeighborhood(cell, neighbors);
        for(U8 i = 0; i < 8; ++i) {
            switch(getType(neighbors[i])) {
                case WATER :
                    freeCell(neighbors[i]);
                    spawnElement(neighbors[i], SEED);
                    neighbors[i]->el->r0 = 1;
                    el->rv = 0;
            }
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 30
    },
    .isStatic = 1,
    .weight = 3,
    .explosionResistance = 2,
    .baseColor = TOHSL(131, 100, 44)
};

#undef ELEMENT_NAME
