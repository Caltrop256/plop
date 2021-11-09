#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME sand

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(cell->temperature >= 893.7f) {
        freeCell(cell);
        spawnElement(cell, GLASS);
        return;
    }
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
        .inertia = 7,
        .impactScatter = 3.0f
    },
    .weight = 4,
    .explosionResistance = 8,
    .baseColor = TOHSL(55, 100, 50)
};

#undef ELEMENT_NAME
