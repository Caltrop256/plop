#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME stone

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(cell->temperature >= 900.0f) {
        freeCell(cell);
        spawnElement(cell, MAGMA);
        return;
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .temp = (TemperatureInfo){
        .insolation = 1.0f
    },

    .state = s_SOLID,
    .weight = 255,
    .isStatic = 1,
    .explosionResistance = 30,
    .baseColor = TOHSL(0, 0, 49)
};

#undef ELEMENT_NAME
