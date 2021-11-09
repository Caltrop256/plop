#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME steam

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target && elementLookup[target->el->type].state == s_PLASMA) {
        swapElements(cell, target);
        return 1;
    };
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0 == 0) {
        freeCell(cell);
        if(cell->temperature < 5.0f) spawnElement(cell, SNOW);
        else spawnElement(cell, WATER);
        return;
    } else if(el->r0 >= 30) {
        Cell *target = getCell(x, y + 1);
        if(getType(target) == EMPTY && randomBool()) spawnElement(target, LIGHTNING);
        el->r0 = 8;
    }
    if(el->rv == 0) el->r0 -= 1;
    if(cell->temperature < 100.0f) el->rv -= 1;

    el->sbpy += (cell->y > (randomU8() % 50 + 20) ? -1 : 1) * 0.4;
    if(cell->x < 10) el->sbpx += 0.4;
    else if(cell->x >= width - 10) el->sbpx -= 0.4;

    if(!attempt_steam(el, cell, getCell(x + RANDDIR, y - randomBool()))) {
        if(randEveryU8(4)) cell->el->color += 1;
    }

    if(el->r0 < 2 && randEveryU32(500000)) {
        Cell *target = getCell(x, y + 1);
        if(getType(target) == EMPTY) spawnElement(target, LIGHTNING);
    }
}

void VARPREF(init)(Element *el) {
    el->r0 = randomU8() % 16 + 8;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),
    .init = VARPREF(init),

    .state = s_GAS,
    .weight = 0.5f,
    .baseColor = TOHSL(0, 0, 75)
};

#undef ELEMENT_NAME