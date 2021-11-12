#ifndef API_H
#define API_H

#include "elements/elements.h"
#include "main.h"

typedef struct IOElement {
    ElementType type;
    U8 rv;
    U8 r0;
    U8 color : 3;
    U8 scorched : 1;
    U8 halted : 1;
    U8 electricityState : 4;
    F32 sbpx;
    F32 sbpy;
} IOElement;

typedef struct IOCell {
    U32 index;
    IOElement el;
} IOCell;

typedef struct IOCanvas {
    char magic[8];
    U8 size;

    U32 cellLength;
    U32 cellSize;
    U32 cellArrStart;

    F32 fvx[N * N];
    F32 fvy[N * N];
    F32 tmp[N * N];

    IOCell cells[];
} IOCanvas;

#endif