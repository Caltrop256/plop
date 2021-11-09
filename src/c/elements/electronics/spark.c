#include "../elements.h"
#include "../../color.h"
#include "../../main.h"
#include "../../random.h"
#include "../../fluidsim.h"
#include "../subatomics.h"

#define ELEMENT_NAME spark

_Bool VARPREF(attempt)(Element *el, Cell *cell, Cell *target) {
    return 0;
}

void VARPREF(update)(Element *el, Cell *cell, U16 x, U16 y) {
    if(el->electricityState != 2) freeCell(cell);
}

void VARPREF(init)(Element *el) {
    el->electricityState = 2;
}    

ElementInfo VARPOST(info) = {
    .attempt = VARPREF(attempt),
    .handler = VARPREF(update),
    .init = VARPREF(init),
    .state = s_UNKNOWN,

    .colors = {
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00),
        COLOR(0xffff00)
    }
};

#undef ELEMENT_NAME
