#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME debris

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    ElementType type = (ElementType)el->r0;
    switch(getType(target)) {
        case EMPTY : moveElement(cell, target);
        case DEBRIS : return 1;
        default :
            if(randEveryU8(10)) {
                freeCell(cell);
                if(randomBool()) {
                    spawnElement(cell, type);
                    cell->el->scorched = 1;
                }
            }
            return 0;
    }
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    ElementInfo *info = &elementLookup[el->r0];

    U8 bvx = (el->rv & 15);
    U8 bvy = (el->rv >> 4);

    el->sbpx += (bvx - 7.5f) / 7.5f * info->weight;
    el->sbpy += (bvy - 7.5f) / 7.5f * info->weight;

    if(randEveryU8(4)) APPROACH(bvx, 10, 0.9);
    if(bvy < 15 && randEveryU8(10)) bvy += 1;
    el->rv = bvx | (bvy << 4);

    if(x == 0 || y == 0 || x == width - 1 || y == height - 1) {
        freeCell(cell);
        if(randomBool()) {
            spawnElement(cell, el->r0);
            cell->el->scorched = 1;
        }
    }
}

void VARPREF(init)(Element *el) {
    el->r0 = SAND;
    el->rv &= 0b00001111;
    el->scorched = 1;
}

ElementInfo VARPOST(info) = {
    .attempt = VARPREF(attempt),
    .handler = VARPREF(update),
    .init = VARPREF(init),

    .state = s_UNKNOWN,
    .isStatic = 1,
    .weight = 0,
    .explosionResistance = 255,
    .baseColor = TOHSL(188, 83, 57)
};

#undef ELEMENT_NAME
