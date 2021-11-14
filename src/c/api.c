#include "main.h"
#include "fluidsim.h"
#include "elements/elements.h"
#include "elements/subatomics.h"
#include "api.h"
#include "walloc.h"
#include "random.h"

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
            if(getType(target) == EMPTY) {
                spawnElement(target, type);
                if(type == ICE) fluid.density[target->fluidInd] = -5.0f;
                else if(type == SNOW) fluid.density[target->fluidInd] = -2.8f;
            }
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

const char *magic = "PLOP :]";

export IOCanvas* exportData(void) {
    U32 len = width * height;
    U32 nonEmptyCells = 0;
    for(U32 i = 0; i < len; ++i) {
        if(cells[i].el) nonEmptyCells += 1;
    }

    IOCanvas *canvas = malloc(sizeof(IOCanvas) + nonEmptyCells * sizeof(IOCell));
    memcpy(canvas->magic, magic, 8);
    canvas->size = width / 75;
    canvas->cellLength = nonEmptyCells;
    canvas->cellSize = sizeof(IOCell);
    canvas->cellArrStart = sizeof(IOCanvas);

    U32 i = len;
    U32 ci = 0;
    while(i --> 0) {
        if(cells[i].el && cells[i].el->type > EMPTY) {
            canvas->cells[ci].index = i;
            canvas->cells[ci].el.type =               cells[i].el->type;
            canvas->cells[ci].el.rv =                 cells[i].el->rv;
            canvas->cells[ci].el.r0 =                 cells[i].el->r0;
            canvas->cells[ci].el.color =              cells[i].el->color;
            canvas->cells[ci].el.halted =             cells[i].el->halted;
            canvas->cells[ci].el.electricityState =   cells[i].el->electricityState;
            canvas->cells[ci].el.sbpx =               cells[i].el->sbpx;
            canvas->cells[ci].el.sbpy =               cells[i].el->sbpy;
            ci += 1;
        }
    }

    U16 fi = N * N;
    while(fi --> 0) {
        canvas->fvx[fi] = fluid.vx[fi];
        canvas->fvy[fi] = fluid.vy[fi];
        canvas->tmp[fi] = fluid.density[fi];
    }

    return canvas;
}

export _Bool importData(IOCanvas *canvas) {
    if(canvas->size == 0 || canvas->size > 20) return 0;
    for(U8 i = 0; i < 8; ++i) {
        if(magic[i] != canvas->magic[i]) return 0;
    }
    for(U32 i = 0; i < canvas->cellLength; ++i) {
        if(canvas->cells[i].el.type >= type_length) {
            return 0;
        } 
    }

    setSize(canvas->size * 75, canvas->size * 75, 1);

    for(U32 i = 0; i < canvas->cellLength; ++i) {
        U32 ti = canvas->cells[i].index;
        spawnElement(&cells[ti], canvas->cells[i].el.type);
        cells[ti].el->rv =               canvas->cells[i].el.rv;
        cells[ti].el->r0 =               canvas->cells[i].el.r0;
        cells[ti].el->color =            canvas->cells[i].el.color;
        cells[ti].el->halted =           canvas->cells[i].el.halted;
        cells[ti].el->electricityState = canvas->cells[i].el.electricityState;
        cells[ti].el->sbpx =             canvas->cells[i].el.sbpx;
        cells[ti].el->sbpy =             canvas->cells[i].el.sbpy;
    }

    U16 fi = N * N;
    while(fi --> 0) {
        fluid.vx[fi] =      canvas->fvx[fi];
        fluid.vy[fi] =      canvas->fvy[fi];
        fluid.density[fi] = canvas->tmp[fi];
    }

    return 1;
}