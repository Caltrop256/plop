#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"

#define ELEMENT_NAME neutron

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    #define REPLACEWITH(x) freeCell(target); spawnElement(target, x)
    switch(getType(target)) {
        case UNBREAKABLECLONER : return 0;
        case ELECTRON : REPLACEWITH(STEAM); freeCell(cell); return 0;
        case EMPTY : moveElement(cell, target); return 1;
        case SEED :
        case PELLET :
        case PLANT : REPLACEWITH(WOOD); break;
        case WOOD : REPLACEWITH(DIRT); break;
        case STONE : if(randEveryU8(30)) {REPLACEWITH(COBBLESTONE);}; break;
        case COBBLESTONE : if(randEveryU8(30)) {REPLACEWITH(WOOD);}; break;
        case DIRT : if(randomBool()) {REPLACEWITH(COAL);}; break;
        case NITRO : REPLACEWITH(OIL); break;
        case PLUTONIUM : if(cell->temperature >= 240.0f && randEveryU8(0xff)) {
            freeCell(target);
            explode(target->x, target->y, 30);
            return 0;
        }
        case POLONIUM : fluid.density[cell->fluidInd] += 5.5f; break;
    }
    #undef REPLACEWITH
    if(target && randEveryU8(25)) {
        swapElements(cell, target);
        return 1;
    }
    if(randEveryU8(10)) {
        freeCell(cell);
        if(randEveryU8(3)) {
            spawnElement(cell, randomBool() ? ELECTRON : PHOTON);
        }
        return 0;
    }
    el->r0 = randomU8();
    el->rv = randomU8();
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    el->sbpx += ((F32)el->r0 - 128.0f) / 128 * 2;
    el->sbpy += ((F32)el->rv - 128.0f) / 128 * 2;

    if(x == 0 || x == width - 1 || y == 0 || y == height - 1) {
        freeCell(cell);
    }
}

void VARPREF(init)(Element *el) {
    el->r0 = randomU8();
}

ElementInfo VARPOST(info) = {
    .attempt = VARPREF(attempt),
    .handler = VARPREF(update),
    .init = VARPREF(init),

    .state = s_UNKNOWN,
    .isStatic = 1,
    .weight = 0,
    .explosionResistance = 1,
    .baseColor = TOHSL(188, 83, 57)
};

#undef ELEMENT_NAME
