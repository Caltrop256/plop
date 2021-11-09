#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME conveyer

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {

    Cell *left = getCell(x - 1, y);
    Cell *right = getCell(x + 1, y);
    U8 elecLeft = !left || !left->el ? 0 : left->el->electricityState;
    U8 elecRight = !right || !right->el ? 0 : right->el->electricityState;

    if(elecLeft == elecRight || elecLeft == 15 || elecRight == 15) return;
    _Bool moveLeft = elecLeft < elecRight;

    for(U8 i = 1; i <= 6; ++i) {
        Cell *target = getCell(x, y - i);
        if(!target) break;
        if(!target->el) continue;

        ElementInfo *info = &elementLookup[target->el->type];
        if(info->isStatic) return;

        if(randEveryU8(20)) {
            APPROACHIFMORE(target->el->sbpy, -info->weight / 2, 0);
        }
        if(!moveLeft) {
            APPROACHIFLESS(target->el->sbpx, info->weight + 1, 0);
        } else {
            APPROACHIFMORE(target->el->sbpx, -(info->weight + 1), 0);
        }
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .weight = 60,
    .isStatic = 1,
    .conductive = 1,
    .explosionResistance = 16,
    .baseColor = TOHSL(93, 16, 29)
};

#undef ELEMENT_NAME
