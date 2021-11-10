#include "subatomics.h"
#include "../main.h"
#include "../walloc.h"
#include "../random.h"
#include "./elements.h"
#include "../fluidsim.h"

#define PI 3.14159265
#define PHI (PI * 2)

U32 nSubatomics = 0;

Subatomic *rootSA = NULL;
Subatomic *last = NULL;

void createSubatomicHelper(U16 x, U16 y, U8 wavelength, U8 angle) {
    if(nSubatomics > MAX_SUBATOMICS) return;
    nSubatomics += 1;
    Subatomic *el = malloc(sizeof (Subatomic));
    el->x = x;
    el->y = y;
    el->waveLength = wavelength;
    el->vx = env_cos(angle) * 2;
    el->vy = env_sin(angle) * 2;
    el->next = NULL;

    if(!rootSA) {
        rootSA = el;
        last = el;
    } else {
        last->next = el;
        last = el;
    }
}

void createSubatomic(U16 x, U16 y, U8 type) {
    U8 dir = randomU8();
    if(type == 0) {
        for(U8 i = 0; i < 7; ++i) createSubatomicHelper(x, y, i, dir);
    } else createSubatomicHelper(x, y, type, dir);
}

void tickSubatomics(void) {
    for(Subatomic *node = rootSA, *prev = NULL; node; prev = node, node = node->next) {
        node->x += node->vx;
        node->y += node->vy;

        if(node->x < 0 || node->y < 0) {
            goto DELETE_PARTICLE;
        }

        Cell *cell = getCell((U16)node->x, (U16)node->y);
        F32 angle;

        if(node->waveLength < 7) {
            ElementType type = getType(cell);
            MatterState state = elementLookup[type].state;
            switch(type) {
                case UNBREAKABLECLONER :
                case CLONER :
                    if(!cell->el->r0) cell->el->r0 = PHOTON;
                default : 
                    if(state == s_PLASMA || state == s_GAS) break;
                case VOID :
                    goto DELETE_PARTICLE;
                case ELECTRON :
                case PHOTON :
                case EMPTY : break;
                case GLASS :
                    angle = env_atan2(node->vy, node->vx);
                    angle += (node->waveLength + 1) / 7.0 * 0.15;
                    node->vx = env_cos(angle) * 2;
                    node->vy = env_sin(angle) * 2;
                    break;
                case PLANT :
                    if(cell->el->rv > 10 && cell->el->rv < 40) {
                        freeCell(cell);
                        spawnElement(cell, SEED);
                        cell->el->r0 = 1;
                    }
                    goto DELETE_PARTICLE;
            }
        } else if(node->waveLength == 0xff) {
            ElementInfo *info = &elementLookup[cell->el->type];
            switch(getType(cell)) {
                default :
                    if(!randEveryU8(info->weight / 2 + 1)) {
                        angle = randomF32() * PHI;
                        node->vx = env_cos(angle) * 2;
                        node->vy = env_sin(angle) * 2;
                    }

                    if(randEveryU32(1024)) cell->el->r0 = randomU8();
                    if(randEveryU32(1024)) cell->el->rv = randomU8();
                    fluid.density[cell->fluidInd] += 0.1f;
                    break;
                case VOID : 
                    goto DELETE_PARTICLE;
                case CLONER :
                    if(randEveryU8(255)) cell->el->r0 = randomU8() % type_length;
                case UNBREAKABLECLONER :
                    if(cell->el->r0 == 0) cell->el->r0 = ELECTRON;
                case ELECTRON :
                case PHOTON :
                case PROTON :
                case DEBRIS :
                case EMPTY : break;
            }
        } else {
            #define COLLISIONLENGTH 7
            ElementType neutronCollision[COLLISIONLENGTH] = {PHOTON, ELECTRON, STEAM, FIRE, WOOD, COBBLESTONE, OIL};
            switch(getType(cell)) {
                case UNBREAKABLECLONER :
                case CLONER :
                    if(!cell->el->r0) cell->el->r0 = PROTON;
                    goto DELETE_PARTICLE;
                default :
                    fluid.density[cell->fluidInd] += 4.0f;
                    if(randEveryU32(1024)) {
                        createSubatomicHelper(cell->x, cell->y, 0xff, randomU8());
                        goto DELETE_PARTICLE;
                    }
                    if(cell->el->electricityState) cell->el->electricityState = 0;
                    break;
                case NEUTRON :
                    freeCell(cell);
                    spawnElement(cell, randEveryU8(3) ? neutronCollision[randomU8() % COLLISIONLENGTH] : neutronCollision[(U8)MAX(0, MIN(COLLISIONLENGTH, cell->temperature / 1000.0f * COLLISIONLENGTH))]);
                case VOID : 
                    goto DELETE_PARTICLE;
                case PROTON :
                case ELECTRON :
                case PHOTON :
                case EMPTY : break;
            }
            #undef COLLISIONLENGTH
        }
        continue;
        DELETE_PARTICLE :
        if(!prev) {
            rootSA = rootSA->next;
            if(!node->next) last = rootSA;
        } else {
            prev->next = node->next;
            if(!node->next) last = prev;
        }
        free(node);
        if(prev) node = prev;
        else node = rootSA;
        nSubatomics -= 1;
        continue;
    }
}