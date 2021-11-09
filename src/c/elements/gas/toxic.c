#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME toxic

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    }
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    el->sbpy += (cell->y > (randomU8() % 50 + 20) ? -1 : 1) * 2.4;
    if(cell->x < 10) el->sbpx += 0.4;
    else if(cell->x >= width - 10) el->sbpx -= 0.4;

    if(!attempt_toxic(el, cell, getCell(x + RANDDIR, y - randomBool()))) {
        if(randEveryU8(4)) cell->el->color += 1;
    }

    update_flammable(el->cell);
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),
    .attempt = VARPREF(attempt),

    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 2,
        .burnTemperature = 70
    },

    .state = s_GAS,
    .weight = 0.4,
    .baseColor = TOHSL(118, 85, 18)
};

#undef ELEMENT_NAME