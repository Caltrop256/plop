#include "main.h"
#include "fluidsim.h"
#include "elements/elements.h"
#include "elements/subatomics.h"
#include "walloc.h"
#include "random.h"
#include "scene.h"

export U32 *imageData;
_Bool voidDelete = 1;

U8 g_tick;
Cell *cells;
U16 width = 0;
U16 height = 0;
ElementInfo elementLookup[type_length];

U8 FSCALE;

export void changeScene(U8 scene) {
    U32 fi = N * N;
    while(fi-->0) {
        fluid.density[fi] = 5.0f;
        fluid.vx[fi] = 0.0f;
        fluid.vy[fi] = 0.0f;
    }
    if(rootSA) {
        for(Subatomic *node = rootSA, *prev = NULL; node->next; prev = node, node = node->next) {
            if(prev) free(prev);
            if(!node->next) free(node);
        }
        rootSA = NULL;
        nSubatomics = 0;
    }
    scenes[scene ? scene - 1 : randomU8() % SCENES]();
}

export void setSize(U16 w, U16 h, _Bool voidScene) {
    if(width) {
        U32 i = width * height;
        while(i --> 0) {
            if(cells[i].el) free(cells[i].el);
        }
        free(cells);
        free(imageData);
    } else {
        fluid.dt = 0.0005f;
        fluid.diff = 0.1f;
        fluid.visc = 0.000000000002f;
        setup_cells();
    }
    
    width = w;
    height = h;
    U32 len = width * height;

    FSCALE = w / N;

    imageData = (U32 *)malloc(len * sizeof (U32));
    cells = (Cell *)malloc(len * sizeof (Cell));

    U32 i = 0;
    for(U16 y = 0; y < height; ++y) {
        for(U16 x = 0; x < width; ++x) {
            cells[i].x = x;
            cells[i].y = y;
            cells[i].cellInd = i;
            cells[i].el = NULL;
            cells[i].temperature = 5.0f;

            F32 hFScale = FSCALE * 0.5f;

            cells[i].fluidNodes[0] = CLAMP((U16)(y - hFScale) / FSCALE, 0, N - 1) * N + CLAMP((U16)(x - hFScale) / FSCALE, 0, N - 1);
            cells[i].fluidNodes[1] = CLAMP((U16)(y - hFScale) / FSCALE, 0, N - 1) * N + CLAMP((U16)(x + FSCALE) / FSCALE, 0, N - 1);
            cells[i].fluidNodes[2] = CLAMP((U16)(y + FSCALE) / FSCALE, 0, N - 1) * N + CLAMP((U16)(x - hFScale) / FSCALE, 0, N - 1);
            cells[i].fluidNodes[3] = CLAMP((U16)(y + FSCALE) / FSCALE, 0, N - 1) * N + CLAMP((U16)(x + FSCALE) / FSCALE, 0, N - 1);

            //if(i == 0) env_log(cells[i].fluidNodes[0], cells[i].fluidNodes[1], cells[i].fluidNodes[2], cells[i].fluidNodes[3]);

            F32 max = 0;
            for(U8 j = 0; j < 4; ++j) {
                U32 fx = (cells[i].fluidNodes[j] % N) * FSCALE;
                U32 fy = (cells[i].fluidNodes[j] / N) * FSCALE;

                cells[i].fluidNodeWeights[j] = (fx - x) * (fx - x) + (fy - y) * (fy - y);
                if(cells[i].fluidNodeWeights[j] > max) {
                    max = cells[i].fluidNodeWeights[j];
                    cells[i].fluidInd = cells[i].fluidNodes[j];
                }
            }

            for(U8 j = 0; j < 4; ++j) {
                cells[i].fluidNodeWeights[j] = 1 - cells[i].fluidNodeWeights[j] / (max + 1);
            }
            i += 1;
        }
    }

    changeScene(voidScene ? 1 : 0);
}

void tickCell(Cell *cell) {
    ElementType type = cell->el == NULL ? EMPTY : cell->el->type;

    APPROACH(cell->temperature, fluid.density[cell->fluidNodes[3]], cell->fluidNodeWeights[3]);
    APPROACH(cell->temperature, fluid.density[cell->fluidNodes[2]], cell->fluidNodeWeights[2]);
    APPROACH(cell->temperature, fluid.density[cell->fluidNodes[1]], cell->fluidNodeWeights[1]);
    APPROACH(cell->temperature, fluid.density[cell->fluidNodes[0]], cell->fluidNodeWeights[0]);

    if(type > EMPTY)  {
        if(cell->el->tick != g_tick) return;
        Element *el = cell->el;
        ElementInfo *info = &elementLookup[type];
        I16 sx = cell->x;
        I16 sy = cell->y;
        cell->el->tick += 1;

        if(info->conductive || el->electricityState) {
            if(!el->electricityState) {
                Cell *neighbors[8];
                getMooreNeighborhood(cell, neighbors);
                for(U8 i = 0; i < 8; ++i) {
                    if(neighbors[i]->el->electricityState == 2) {
                        if(neighbors[i]->el->type == CONVEYER) {
                            if(type == CONVEYER) el->electricityState = 1;
                        } else el->electricityState = 1;
                        break;
                    }
                }
            } else el->electricityState += 1;
        }

        F32 fvx = fluid.vx[cell->fluidInd];
        F32 fvy = fluid.vy[cell->fluidInd];

        if(info->state != s_UNKNOWN && fvx * fvx + fvy * fvy >= info->weight * info->weight) {
            cell->el->sbpx += fvx - (info->weight * SIGN(fvx));
            cell->el->sbpy += fvy - (info->weight * SIGN(fvy));
        }

        if(info->weight) {
            fluid.vx[cell->fluidInd] -= info->weight * SIGN(fvx) * 0.0002;
            fluid.vy[cell->fluidInd] -= info->weight * SIGN(fvy) * 0.0002;
        }

        I16 mx = 0, my = 0;

        if(info->attempt && (ABS(cell->el->sbpx) >= 1 || ABS(cell->el->sbpy) >= 1)) {
            if(info->dontLimitSubPixelMovementSpeed) {
                mx = (I16)cell->el->sbpx;
                my = (I16)cell->el->sbpy;
            } else {
                mx = MAX(-8, MIN(8, (I16)cell->el->sbpx));
                my = MAX(-8, MIN(8, (I16)cell->el->sbpy));
            }

            if(!mx) {
                moveVertically(cell, my, info->attempt);
            } else if(!my) {
                moveHorizontally(cell, mx, info->attempt);
            } else {
                lineMove(cell->x, cell->y, cell->x + mx, cell->y + my, info->attempt);
            }
            cell = el->cell;
        }

        if(info->handler) info->handler(cell->el, cell, cell->x, cell->y);

        if(mx || my) {
            el->sbpx -= mx;
            el->sbpy -= my;
        }

        cell = el->cell;
        if(info->weight) {
            I8 dirX = cell->x - sx;
            I8 dirY = cell->y - sy;
            if(dirX > 0) {
                APPROACHIFLESS(fluid.vx[cell->fluidInd], info->weight * 1.1, 1 / (dirX));
            } else if(dirX < 0) {
                APPROACHIFMORE(fluid.vx[cell->fluidInd], info->weight * -1.1, 1 / -(dirX));
            }
            if(dirY > 0) {
                APPROACHIFLESS(fluid.vy[cell->fluidInd], info->weight * 1.1, 1 / (dirY));
            } else if(dirY < 0) {
                APPROACHIFMORE(fluid.vy[cell->fluidInd], info->weight * -1.1, 1 / -(dirY));
            }
        }

        if(info->state == s_SOLID && !info->temp.flammable && cell->temperature >= 500.0f) {
            el->scorched = 1;
        }

        if(!info->isStatic) {
            switch(info->state) {
                case s_LIQUID :
                    APPROACHIFLESS(el->sbpy, 100, 0.99);
                    break;
                case s_SOLID :
                    APPROACHIFLESS(el->sbpy, 100, 0.98);
                    break;
            }  
        }

        APPROACHIFMORE(fluid.density[cell->fluidInd], cell->temperature, 1 - (info->temp.insolation));
    } else {
        if(cell->x == 0 || cell->y == 0 || cell->x == width - 1 || cell->y == height - 1) {
            APPROACH(fluid.density[cell->fluidInd], 5.0f, 0.999);
        }
    }
}

void loopThroughAllCells() {
    U32 i = width * height;
    U16 y = height;
    while(y-->0) {
        if(randomBool()) {
            U16 x = width;
            while(x-->0) {
                i -= 1;
                tickCell(&cells[i]);
            }
        } else {
            i -= width;
            for(U16 x = 0; x < width; ++x) tickCell(&cells[i + x]);
        }
    }
}

export void tick() {
    explosionPower = 0;
    tickSubatomics();
    loopThroughAllCells();
    loopThroughAllCells();
    stepFluid();
    g_tick += 1;
}


void line(U16 x0, U16 y0, U16 x1, U16 y1, U32 clr) {
    int dx = ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
    int dy = -ABS(y1 - y0), sy = y0 < y1 ? 1 : -1; 
    int err = dx + dy, e2;
    
    for(;;) {
        if(y0 < 0 || y0 >= height || x0 < 0 || x0 >= width) break;
        imageData[y0 * width + x0] |= clr;
        if(x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if(e2 >= dy) { err += dy; x0 += sx; }
        if(e2 <= dx) { err += dx; y0 += sy; }
    }
}

export void draw() {
    U32 i = width * height;
    while(i --> 0) {
        imageData[i] = 0xff << 24;
        Element *el = cells[i].el;
        if(el) {
            Color clr;
            switch(el->type) {
                case DEBRIS :
                    clr = elementLookup[el->r0].colors[el->color];
                    break;
                case PUMP :
                    if(el->r0) {
                        clr = elementLookup[el->r0].colors[el->rv];
                        break;
                    }
                default : 
                    clr = elementLookup[el->type].colors[el->color];
                    break;
            }
            if(el->electricityState == 1) imageData[i] |= 0x00ffff;
            else if(!el->scorched) imageData[i] |= clr.value;
            else imageData[i] |= (clr.r & 63) | ((clr.g & 63) << 8) | ((clr.b & 63) << 16);
        }

        F32 temperature = cells[i].temperature;

        if(temperature >= 0) {
            temperature *= 0.5;
            if(temperature >= 50 && temperature < 130) temperature = 50;
            else if(temperature >= 130) temperature -= 80;
            imageData[i] |= (U8)MIN(255, temperature);
        } else imageData[i] |= ((U8)MIN(255, temperature * -4)) << 16;
    }
    i = 0;
    for(U16 y = 0; y < height; ++y) {
        for(U16 x = 0; x < width; ++x) {
            F32 fvx = fluid.vx[cells[i].fluidInd] * 0.5;
            F32 fvy = fluid.vy[cells[i].fluidInd] * 0.5;

            if((ABS(fvx) >= 1 || ABS(fvy) >= 1) && x % 10 == 0 && y % 10 == 0) {
                U8 val = MIN(255, (fvx * fvx + fvy * fvy));
                Color clr = {.a = 0xff, .r = val, .g = val, .b = val};
                line(x, y, x + fvx, y + fvy, clr.value);
            }
            i += 1;
        }
    }

    Color c[7] = {
        {.a=0xff,.r=0xff,.g=0,.b=0},
        {.a=0xff,.r=0xff,.g=0xa5,.b=0},
        {.a=0xff,.r=0xff,.g=0xff,.b=0},
        {.a=0xff,.r=0,.g=0x80,.b=0},
        {.a=0xff,.r=0,.g=0,.b=0xff},
        {.a=0xff,.r=0x4b,.g=0,.b=0x82},
        {.a=0xff,.r=0xee,.g=0x82,.b=0xee}
    };
    for(Subatomic *node = rootSA; node; node = node->next) {
        U32 ind = (U16)node->y * width + (U16)node->x;
        if(ind >= width * height) break;
        if(node->waveLength == 0xff) imageData[ind] = c[0].value;
        else if(node->waveLength == 0xfe) imageData[ind] = 0xff0DD061;
        else imageData[ind] |= c[node->waveLength].value;
    }
}

void *memcpy(void *dest, const void *src, unsigned long s) {
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    while(s --> 0) cdest[s] = csrc[s];
    return dest;
}