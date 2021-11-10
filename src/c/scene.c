#include "elements/elements.h"
#include "main.h"
#include "random.h"
#include "scene.h"
#include "fluidsim.h"

void void_scene(void) {
    U32 i = width * height;
    while(i --> 0) {
        Cell *target = &cells[i];
        if(getType(target) != EMPTY) freeCell(target);
    }
};

void dirt_scene(void) {
    U32 i = width * height;
    while(i --> 0) {
        Cell *target = &cells[i];
        if(getType(target) != EMPTY) freeCell(target);

        if(target->y >= height - 100 && randEveryU8(4)) spawnElement(target, target->y >= height - 50 ? COBBLESTONE : DIRT);
    }

    Cell *seedCell = getCell(randomU32() % width, 0);
    if(getType(seedCell) == EMPTY) spawnElement(seedCell, SEED);
}

void snow_scene(void) {
    U32 i = width * height;
    while(i --> 0) {
        Cell *target = &cells[i];
        if(getType(target) != EMPTY) freeCell(target);
        fluid.density[target->fluidInd] = -4.0f;

        if(target->y >= height - 100 && randEveryU8(4)) spawnElement(target, target->y >= height - 50 ? DIRT : SNOW);
        if(target->y < 30 && randEveryU8(4)) {
            spawnElement(target, STEAM);
            target->el->r0 = randomU8() % 8 + 4;
        }
    }
}

void sand_scene(void) {
    U32 i = width * height;
    _Bool right = randomBool();
    while(i --> 0) {
        Cell *target = &cells[i];
        if(getType(target) != EMPTY) freeCell(target);

        if(target->y >= height - 100 && randEveryU8(4)) {
            if((!right && target->x >= width / 3) || (right && target->x <= width - width / 3)) spawnElement(target, SAND);
            else spawnElement(target, WATER);
        }
    }
}

inline F32 nonPeriodicWave(F32 x) {
    return env_sin(x * 2) + env_sin(x * 3.14159265);
}

U16 mountainSample(F32 offset, U16 x) {
    F32 y = 0.0f;

    for(U8 i = 1; i <= 10; ++i) {
        F32 samplePoint = x / (width * (1.0f / i)) + offset;
        y += ABS(nonPeriodicWave(samplePoint)) * 50 * (1.0f / i);
    }

    return height - (U16)y;
}

void mountain_scene(void) {
    void_scene();

    F32 offset = randomF32() * 1000;

    for(U16 x = 0; x < width; ++x) {
        U16 y = mountainSample(offset, x);
        U16 oy = y;
        while(y < height) {
            Cell *target = &cells[x + y * width];
            U16 diff = y - oy;

            if(y < 300) fluid.density[target->fluidInd] = -5.0f;

            if(diff < 8) {
                spawnElement(target, y >= 280 ? DIRT : SNOW);
            } else if(diff < 80 && y < 280) {
                spawnElement(target, randEveryU8(4) ? SNOW : ICE);
            } else spawnElement(target, STONE);
            y += 1;
        }
    }
}

void (*scenes[SCENES])(void) = {
    void_scene,
    dirt_scene,
    snow_scene,
    sand_scene,
    mountain_scene
};