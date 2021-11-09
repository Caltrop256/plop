#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME coal

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!default_attempt(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(default_attempt(el, cell, getCell(x + dir, y + 1)));
            else if(default_attempt(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
    update_flammable(el->cell);
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 14,
        .impactScatter = 1.8f
    },
    .temp = (TemperatureInfo){
        .flammable = 1,
        .burnTime = 5,
        .burnTemperature = 245
    },
    .weight = 6.6,
    .baseColor = TOHSL(0, 0, 6)
};

#undef ELEMENT_NAME
