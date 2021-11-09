#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME piston

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    Cell *targetLeft = getCell(x - 1, y);
    ElementType targetType = getType(targetLeft);

    switch(el->r0) {
        case 0 :
            if(el->electricityState == 1) el->rv = 30;

            if(el->rv > 0) {
                el->rv -= 1;
                if(targetType == EMPTY) {
                    spawnElement(targetLeft, PISTON);
                    targetLeft->el->r0 = 1;
                    targetLeft->el->rv = 30;
                    targetLeft->el->color = randomU8() % 4 + 4;
                }
            } else if(targetType == PISTON && targetLeft->el->r0 == 1) targetLeft->el->r0 = 2;
            break;
        case 1 :
            if(el->rv >= 20) el->electricityState = 0;
            if(!el->rv) break;
            if(targetType == EMPTY) {
                spawnElement(targetLeft, PISTON);
                targetLeft->el->r0 = 1;
                targetLeft->el->rv = el->rv - 1;
                targetLeft->el->color = randomU8() % 4 + 4;
            } else {
                for(U8 i = 1; i < el->rv; ++i) {
                    Cell *target = getCell(x - i, y);
                    ElementType type = getType(target);
                    if(type == VOID) break;
                    else if(type == EMPTY) continue;
                    ElementInfo *info = &elementLookup[type];
                    if(info->isStatic) break;
                    else {
                        APPROACHIFMORE(target->el->sbpx, -(info->weight) - 1, 0);
                    }
                }
            }
            break;
        case 2 :
            el->electricityState = 0;
            if(el->rv == 0) {
                freeCell(cell);
                return;
            }
            el->rv -= 1;
            if(targetType == PISTON && targetLeft->el->r0 == 1) targetLeft->el->r0 = 2; 
    }
}

void VARPREF(init)(Element *el) {
    el->rv = 0;
    el->color = randomU8() % 4;
}

ElementInfo VARPOST(info) = {
    .attempt = VARPREF(attempt),
    .handler = VARPREF(update),
    .init = VARPREF(init),

    .state = s_SOLID,
    .weight = 60,
    .isStatic = 1,
    .conductive = 1,
    .explosionResistance = 16,
    .colors = {
        COLOR(0x3e463e),
        COLOR(0x313a31),
        COLOR(0x1f271f),
        COLOR(0x4a514a),

        COLOR(0x735c21),
        COLOR(0x5f4b16),
        COLOR(0x57471d),
        COLOR(0x625227)
    }
};

#undef ELEMENT_NAME
