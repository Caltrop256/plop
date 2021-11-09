#ifndef COLOR_H
#define COLOR_H

#include "./main.h"

#define COLOR(n) ((Color)(U32)((0xff << 24) | ((n & 0xff) << 16) | (n & 0xff00) | ((n & 0xff0000) >> 16)))
typedef union Color {
    struct {
        U8 r;
        U8 g;
        U8 b;
        U8 a;
    };
    U32 value;
} Color;

#define TOHSL(h, s, l) ((HSL){.hue = h / 360.0f, .sat = s / 100.0f, .lig = l / 100.0f})
typedef struct HSL {
    F32 hue;
    F32 sat;
    F32 lig;
} HSL;

inline U8 toU8(F32 n);
F32 hueToRGB(F32 p, F32 q, F32 t);
Color hslToRgb(HSL hsl);

#endif
