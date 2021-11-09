#include "color.h"

U8 toU8(F32 n) {
    return MIN(255, n * 256);
}

F32 hueToRGB(F32 p, F32 q, F32 t) {
    if (t < 0.0f) t += 1.0f;
    if (t > 1.0f) t -= 1.0f;
    if (t < 1.0 / 6.0f) return p + (q - p) * 6.0f * t;
    if (t < 1.0 / 2.0f) return q;
    if (t < 2.0 / 3.0f) return p + (q - p) * (2.0f / 3.0f - t) * 6.0f;
    return p;
}

Color hslToRgb(HSL hsl) {
    Color out = {.a = 0xff};

    if (hsl.sat == 0.0f) out.r = out.g = out.b = toU8(hsl.lig);
    else {
        F32 q = hsl.lig < 0.5f ? hsl.lig * (1 + hsl.sat) : hsl.lig + hsl.sat - hsl.lig * hsl.sat;
        F32 p = 2 * hsl.lig - q;

        out.r = toU8(hueToRGB(p, q, hsl.hue + 1.0f / 3.0f));
        out.g = toU8(hueToRGB(p, q, hsl.hue));
        out.b = toU8(hueToRGB(p, q, hsl.hue - 1.0f / 3.0f));
    }
    return out;
}