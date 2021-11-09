#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME fungus

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    Cell *n[8];
    getMooreNeighborhood(cell, n);
    for(U8 i = 0; i < 8; ++i) {
        Cell *target = n[i];
        if(target && target->el) {
            ElementInfo *info = &elementLookup[target->el->type];
            switch(target->el->type) {
                case FUNGUS :
                    if(randEveryU8(4 * 8)) el->color += 1;
                    break;
                default :
                    if(info->state == s_SOLID && randEveryU8(info->weight * 4 + 15)) {
                        freeCell(target);
                        spawnElement(target, FUNGUS);
                    }
            }
        }
    }

    update_flammable(cell);
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 3,
        .burnTemperature = 90
    },
    .isStatic = 1,
    .weight = 3,
    .explosionResistance = 2,
    .baseColor = TOHSL(290, 69, 65)
};

#undef ELEMENT_NAME
