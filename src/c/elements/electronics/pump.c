#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME pump

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->electricityState == 4) {
        Cell *n[8];
        getMooreNeighborhood(cell, n);

        for(U8 i = 0; i < 8; ++i) {
            U8 j = randomU8() % 8;
            Cell *t = n[i];
            n[i] = n[j];
            n[j] = t;
        }

        if(el->r0 == 0) {
            for(U8 i = 0; i < 8; ++i) {
                Cell *target = n[i];
                ElementInfo *info;
                switch(getType(target)) {
                    case VOID :
                    case EMPTY :
                    case PUMP :
                        continue;

                    default :
                        info = &elementLookup[target->el->type];
                        if(!info->isStatic) {
                            el->r0 = target->el->type;
                            el->rv = target->el->color;
                            freeCell(target);
                            return;
                        }
                }
            }
        } else {
            for(U8 i = 0; i < 8; ++i) {
                Cell *target = n[i];
                if(getType(target) == PUMP && target->el->electricityState < 4) {
                    if(target->el->r0) {
                        update_pump(target->el, target, target->x, target->y);
                    } else {
                        target->el->r0 = el->r0;
                        target->el->rv = el->rv;
                        el->r0 = 0;
                        el->rv = 0;
                    }
                    break;
                }
            }
            for(U8 i = 0; i < 8; ++i) {
                Cell *target = n[i];
                if(getType(target) == EMPTY) {
                    spawnElement(target, el->r0);
                    target->el->color = el->rv;
                    el->r0 = 0;
                    el->rv = 0;
                }
            }
        }
    } else if(el->electricityState == 6) el->electricityState = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .weight = 60,
    .isStatic = 1,
    .conductive = 1,
    .explosionResistance = 16,
    .baseColor = TOHSL(25, 74, 16)
};

#undef ELEMENT_NAME
