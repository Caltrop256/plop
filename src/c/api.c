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

export U32 exportByteLen(void) {
    return sizeof(IOCanvas) + width * height * sizeof(IOCell);
}

export IOCanvas* exportData(void) {
    U32 len = width * height;
    IOCanvas *canvas = malloc(sizeof(IOCanvas) + len * sizeof(IOCell));
    canvas->magic = 0xB00B;
    canvas->width = width;
    canvas->height = height;

    U32 i = len;
    while(i --> 0) {
        canvas->cells[i].nElectrons = 0;
        canvas->cells[i].nProtons = 0;
        canvas->cells[i].nPhotons = 0;
        if(cells[i].el) {
            canvas->cells[i].el.type =               cells[i].el->type;
            canvas->cells[i].el.rv =                 cells[i].el->rv;
            canvas->cells[i].el.r0 =                 cells[i].el->r0;
            canvas->cells[i].el.color =              cells[i].el->color;
            canvas->cells[i].el.halted =             cells[i].el->halted;
            canvas->cells[i].el.electricityState =   cells[i].el->electricityState;
            canvas->cells[i].el.sbpx =               cells[i].el->sbpx;
            canvas->cells[i].el.sbpy =               cells[i].el->sbpy;
        } else {
            canvas->cells[i].el.type =             0;
            canvas->cells[i].el.rv =               0;
            canvas->cells[i].el.r0 =               0;
            canvas->cells[i].el.color =            0;
            canvas->cells[i].el.halted =           0;
            canvas->cells[i].el.electricityState = 0;
            canvas->cells[i].el.sbpx =             0;
            canvas->cells[i].el.sbpy =             0;
        }
    }

    U16 fi = N * N;
    while(fi --> 0) {
        canvas->fvx[fi] = fluid.vx[fi];
        canvas->fvy[fi] = fluid.vy[fi];
        canvas->tmp[fi] = fluid.density[fi];
    }

    if(rootSA) {
        for(Subatomic *node = rootSA; node->next; node = node->next) {
            U32 ind = (U16)node->y * width + (U16)node->x;
            switch(node->waveLength) {
                case 0xff : canvas->cells[ind].nElectrons += 1; break;
                case 0xfe : canvas->cells[ind].nProtons += 1; break;
                default : canvas->cells[ind].nPhotons += 1; break;
            }
        }
    }

    return canvas;
}

export void importData(IOCanvas *canvas) {
    setSize(canvas->width, canvas->height, 0);
    U32 i = width * height;
    while(i --> 0) {
        if(cells[i].el) freeCell(&cells[i]);

        if(canvas->cells[i].el.type) {
            spawnElement(&cells[i], canvas->cells[i].el.type);
            cells[i].el->rv =               canvas->cells[i].el.rv;
            cells[i].el->r0 =               canvas->cells[i].el.r0;
            cells[i].el->color =            canvas->cells[i].el.color;
            cells[i].el->halted =           canvas->cells[i].el.halted;
            cells[i].el->electricityState = canvas->cells[i].el.electricityState;
            cells[i].el->sbpx =             canvas->cells[i].el.sbpx;
            cells[i].el->sbpy =             canvas->cells[i].el.sbpy;
        }

        while(canvas->cells[i].nElectrons --> 0) createSubatomic(cells[i].x, cells[i].y, 0xff);
        while(canvas->cells[i].nProtons --> 0) createSubatomic(cells[i].x, cells[i].y, 0xfe);
        while(canvas->cells[i].nPhotons --> 0) createSubatomic(cells[i].x, cells[i].y, randomU8() % 7);
    }

    U16 fi = N * N;
    while(fi --> 0) {
        fluid.vx[fi] =      canvas->fvx[fi];
        fluid.vy[fi] =      canvas->fvy[fi];
        fluid.density[fi] = canvas->tmp[fi];
    }
}