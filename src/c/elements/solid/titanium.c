#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME titanium

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .temp = (TemperatureInfo){
        .insolation = 1.7f
    },

    .state = s_SOLID,
    .weight = 255,
    .isStatic = 1,
    .explosionResistance = 78,
    .baseColor = TOHSL(0, 0, 84)
};

#undef ELEMENT_NAME
