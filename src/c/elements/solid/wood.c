#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME wood

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0 && el->r0 == 2 && randEveryU8(50)) {
        freeCell(cell);
        spawnElement(cell, COAL);
        return;
    }
    update_flammable(cell);
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 5,
        .burnTemperature = 175,
    },

    .state = s_SOLID,
    .weight = 14,
    .isStatic = 1,
    .explosionResistance = 9,
    .baseColor = TOHSL(35, 99, 16)
};

#undef VARIABLE
