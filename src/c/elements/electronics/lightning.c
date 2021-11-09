#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define PI 3.14159265
#define PHI (PI * 2)

#define ELEMENT_NAME lightning

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(el->r0) return 0;

    Cell *rand = getCell(cell->x + RANDDIR, cell->y + RANDDIR);
    _Bool isEmpty = getType(rand) == EMPTY;

    ElementInfo* info = &elementLookup[target->el->type];

    switch(getType(target)) {
        case WATER : if(isEmpty) spawnElement(rand, STEAM);
        case PHOTON :
        case ELECTRON :
        case NEUTRON : freeCell(target);
        case EMPTY : 
            spawnElement(target, LIGHTNING);
            el->r0 += 1;
            return 1;

        case BATTERY :
            freeCell(target);
            explode(target->x, target->y, 40);
            break;

        default : 
            if(info->state == s_GAS || info->state == s_PLASMA) {
                freeCell(target);
                spawnElement(target, LIGHTNING);
                el->r0 += 1;
                return 1;
            }
            break;
    }
    freeCell(cell);
    explode(cell->x, cell->y, 2);
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    el->sbpx += randomF32() * 30 - 15;
    el->sbpy += 40;

    Cell *neighbors[8];
    getMooreNeighborhood(cell, neighbors);
    for(U8 i = 0; i < 8; ++i) if(neighbors[i] && neighbors[i]->el) neighbors[i]->el->electricityState = 1;

    if(el->r0 && el->rv-- == 0) {
        freeCell(cell);
        if(randEveryU8(4)) spawnElement(cell, SMOKE);
        return;
    }
    if(x == 0 || x == width - 1 || y == 0 || y == height - 1) freeCell(cell);
}

void VARPREF(init)(Element *el) {
    el->color = 0;
    el->rv %= 8;
}

ElementInfo VARPOST(info) = {
    .attempt = VARPREF(attempt),
    .handler = VARPREF(update),
    .init = VARPREF(init),

    .state = s_PLASMA,
    .isStatic = 1,
    .dontLimitSubPixelMovementSpeed = 1,
    .weight = 255,
    .explosionResistance = 1,
    .colors = {
        COLOR(0xffffff),
    }
};

#undef ELEMENT_NAME
