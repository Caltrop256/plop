#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME dirt

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(!default_attempt(el, cell, getCell(x, y + 1))) {
        if(!el->halted) {
            int8_t dir = LORDIR;
            if(default_attempt(el, cell, getCell(x + dir, y + 1)));
            else if(default_attempt(el, cell, getCell(x - dir, y + 1)));
            else attemptHalt(el);
        } else tryReleaseHalted(cell);
    } else el->halted = 0;
}

ElementInfo VARPOST(info) = {
    .handler = VARPREF(update),

    .state = s_SOLID,
    .solid = (SolidInfo){
        .inertia = 9,
        .impactScatter = 2.3f
    },
    .weight = 4.6,
    .explosionResistance = 17,
    .baseColor = TOHSL(23, 49, 26)
};

#undef ELEMENT_NAME
