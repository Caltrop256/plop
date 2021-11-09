#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"
#include "../subatomics.h"

#define ELEMENT_NAME plutonium

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {

    if(randEveryU32(1024) || randomF32() < cell->temperature / 290) {
        Cell *target = getCell(x + RANDDIR, y + RANDDIR);
        if(getType(target) == EMPTY) {
            if(randEveryU8(4)) spawnElement(target, NEUTRON);
            el->rv -= 1;
        } else if((getType(target) == POLONIUM || getType(target) == PLUTONIUM) && randEveryU32(300)) {
            freeCell(target);
            spawnElement(target, NEUTRON);
            el->rv -= 4;
        } else if(randEveryU8(9)) {
            createSubatomic(x, y, randomBool() ? 0xfe : 0xff);
            el->rv -= 1;
        }
    }

    if(el->rv == 0) {
        freeCell(cell);
        spawnElement(cell, COBBLESTONE);
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
        .inertia = 3,
        .impactScatter = 3.0f
    },
    .weight = 6,
    .explosionResistance = 8,
    .baseColor = TOHSL(93, 82, 49)
};

#undef ELEMENT_NAME
