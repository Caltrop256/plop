#ifndef SUBATOMICS_H
#define SUBATOMICS_H

#include "../color.h"

void tickSubatomics(void);
void createSubatomic(U16 x, U16 y, U8);
void createSubatomicHelper(U16 x, U16 y, U8 wavelength, U8 angle);

typedef struct Subatomic {
    struct Subatomic *next;

    F32 x;
    F32 y;
    F32 vx;
    F32 vy;
    U8 waveLength;
} Subatomic;

#define MAX_SUBATOMICS 100000
extern Subatomic *rootSA;
extern U32 nSubatomics;

#endif