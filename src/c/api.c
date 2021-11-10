#include "main.h"
#include "fluidsim.h"
#include "elements/elements.h"
#include "elements/subatomics.h"

export F32 getTemp(U16 x, U16 y) {
    return cells[y * width + x].temperature;
}

export void fluidVelocity(U16 x, U16 y, F32 vx, F32 vy) {
    Cell *target = getCell(x, y);
    if(target) {
        fluid.vx[target->fluidInd] += vx;
        fluid.vy[target->fluidInd] += vy;
    }
}

export U32 getNSubatomics(void) {
    return nSubatomics;
}

export void applyPaint(U16 mx, U16 my, ElementType type, U8 areaOfEffect) {
    if((type == PHOTON || type == ELECTRON || type == PROTON) && nSubatomics >= MAX_SUBATOMICS) return;

    for(I16 y = -areaOfEffect; y <= areaOfEffect; ++y) {
        I32 py = my + y;
        if(py < 0) continue;
        else if(py >= height) break;

        for(I16 x = -areaOfEffect; x <= areaOfEffect; ++x) {
            I32 px = mx + x;
            if(px < 0) continue;
            else if(px >= width) break;

            if(areaOfEffect > 3 && y * y + x * x >= areaOfEffect * areaOfEffect) continue;

            Cell *target = getCell(px, py);
            if(getType(target) == EMPTY) spawnElement(target, type);
        }
    }
}

export void eraseArea(U16 mx, U16 my, U8 areaOfEffect) {
    for(I16 y = -areaOfEffect; y <= areaOfEffect; ++y) {
        I32 py = my + y;
        if(py < 0) continue;
        else if(py >= height) break;

        for(I16 x = -areaOfEffect; x <= areaOfEffect; ++x) {
            I32 px = mx + x;
            if(px < 0) continue;
            else if(px >= width) break;

            if(areaOfEffect > 3 && y * y + x * x >= areaOfEffect * areaOfEffect) continue;

            Cell *target = getCell(px, py);
            if(getType(target) > EMPTY) freeCell(target);
        }
    }
}