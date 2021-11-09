#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME seed

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(el->r0) {
        Cell *spawnA;
        switch(getType(target)) {
            default : return 0;
            case WATER :
            case SEED :
                freeCell(target);
            case EMPTY : 
                moveElement(cell, target);
                spawnElement(cell, PLANT);
                break;
            case WOOD :
                if(randEveryU8(50)) {
                    freeCell(target);
                    moveElement(cell, target);
                    spawnElement(cell, PLANT);
                    return 1;
                } else {
                    spawnA = getCell(cell->x + RANDDIR, cell->y + RANDDIR);
                    switch(getType(spawnA)) {
                        case PELLET :
                        case PLANT :
                        case SEED :
                            freeCell(spawnA);
                        case EMPTY :
                            spawnElement(spawnA, SEED);
                            spawnA->el->r0 = 1;
                            break;
                    }
                    return 0;
                }
        };
    } else return default_attempt(el, cell, target);
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(cell->temperature <= 3.4f) {
        freeCell(cell);
        spawnElement(cell, DIRT);
        return;
    };

    ElementType typeBelow = getType(getCell(x, y + 1));
    if(typeBelow == PLANT && !el->r0) {
        freeCell(cell);
        return;
    }

    if(el->r0 || (el->halted && (typeBelow == SAND || typeBelow == DIRT))) {
        el->sbpx = 0.0f;
        el->sbpy = 0.0f;
        el->r0 += 1;
        if(el->r0 == el->rv) {
            freeCell(cell);
            Cell *neighbors[8];
            getMooreNeighborhood(cell, neighbors);
            for(U8 i = 0; i < 8; ++i)
                if(getType(neighbors[i]) == EMPTY && randEveryU8(1)) spawnElement(neighbors[i], PELLET);
            return;
        }
        attempt_seed(el, cell, getCell(x + RANDDIR, y - 1));
        if(randEveryU8(8)) {
            Cell *target = getCell((randomBool() ? RANDDIR : 0) + x, y);
            if(getType(target) == EMPTY) {
                spawnElement(target, SEED);
                target->el->r0 = 1;
            }
        }
    } else if(!attempt_seed(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(attempt_seed(el, cell, getCell(x + dir, y + 1)));
            else if(attempt_seed(el, cell, getCell(x - dir, y + 1)));
            else el->halted = 1;
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

void VARPREF(init)(Element *el) {
    el->rv %= 15 + 20;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),
    .init = VARPREF(init),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 90,
        .impactScatter = 0.0f
    },
    .weight = 0.1,
    .explosionResistance = 2,
    .baseColor = TOHSL(39, 93, 34)
};

#undef ELEMENT_NAME
