#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME dynamite

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->r0 || el->electricityState) {
        freeCell(cell);
        explode(x, y, el->rv % 30);
    }
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .temp = (TemperatureInfo){
        .insolation = 1.0f,
        .flammable = 1,
        .burnTime = 30
    },

    .state = s_SOLID,
    .weight = 30,
    .isStatic = 1,
    .explosionResistance = 8,
    .baseColor = TOHSL(0, 73, 63)
};

#undef ELEMENT_NAME
