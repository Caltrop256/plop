#include "elements.h"
#include "../random.h"
#include "../walloc.h"
#include "../fluidsim.h"

export const char *enumToString[] = {
    #define GENERATE_STRING(UPPER,LOWER) #UPPER,
    FOREACH_ELEMENTTYPE(GENERATE_STRING)
    #undef GENERATE_STRING
    NULL
};

export void freeCell(Cell *c) {
    if(c && c->el) {
        free((void *)c->el);
        c->el = NULL;
    }
}

export void spawnElement(Cell *cell, ElementType type) {
    Element *el = malloc(sizeof (Element));
    el->type = type;
    el->scorched = 0;
    el->halted = 0;
    el->color = randomU8() % 8;
    el->rv = randomU8();
    el->r0 = 0;
    el->tick = g_tick + 1;
    el->cell = cell;
    el->sbpx = 0.0f;
    el->sbpy = 0.0f;
    el->electricityState = 0;

    ElementInfo info = elementLookup[type];
    cell->el = el;
    if(info.init) info.init(el);
}

void moveElement(Cell *source, Cell *target) {
    target->el = source->el;
    source->el = NULL;
    target->el->cell = target;
}

void swapElements(Cell *source, Cell *target) {
    Element *sourcePtr = source->el;
    Element *targetPtr = target->el;

    source->el = targetPtr;
    target->el = sourcePtr;

    source->el->cell = source;
    target->el->cell = target;
}

export Cell *getCell(I32 x, I32 y) {
    if(x < 0 || x >= width || y < 0 || y >= height) return NULL;
    return &cells[y * width + x];
}

void getMooreNeighborhood(Cell *c, Cell **out) {
    U16 x = c->x;
    U16 y = c->y;
    U32 i = c->cellInd;

    out[0] = NULL;
    out[1] = NULL;
    out[2] = NULL;
    out[3] = NULL;
    out[4] = NULL;
    out[5] = NULL;
    out[6] = NULL;
    out[7] = NULL;

    if(y > 0) {
        if(x) out[0] = &cells[(i - width) - 1]; // top left
        out[1] = &cells[i - width]; // top
        if(x < width - 1) out[2] = &cells[(i - width) + 1]; // top right
    }

    if(x > 0) out[3] = &cells[i - 1]; // left
    if(x < width - 1) out[4] = &cells[i + 1]; // right

    if(y < height - 1) {
        if(x > 0) out[5] = &cells[(i + width) - 1]; // bottom left
        out[6] = &cells[i + width]; // bottom
        if(x < width - 1) out[7] = &cells[i + width + 1]; // bottom right
    }
}

export inline ElementType getType(Cell *c) {
    return c == NULL ? VOID : c->el == NULL ? EMPTY : c->el->type;
}

void moveHorizontally(Cell *source, I16 distance, _Bool (*callback)(Element *, Cell *, Cell *)) {
    if(distance == 0) return;

    U16 dx;
    U16 x = source->x;
    U32 ind = source->cellInd;

    if(distance < 0 && x < -distance) dx = 0;
    else if(distance > 0 && width - x <= distance) dx = width - 1;
    else dx = x + distance;

    I8 dir = distance < 0 ? -1 : 1;
    Cell *target;

    while(x != dx) {
        ind += dir;
        x += dir;
        target = &cells[ind];
        if(!callback(source->el, source, target)) break;
        source = target;
    }
}

void moveVertically(Cell *source, I16 distance, _Bool (*callback)(Element *, Cell *, Cell *)) {
    if(distance == 0) return;

    U16 dy;
    U16 y = source->y;
    U32 ind = source->cellInd;

    if(distance < 0 && y < -distance) dy = 0;
    else if(distance > 0 && height - y <= distance) dy = height - 1;
    else dy = y + distance;

    I8 dir = distance < 0 ? -1 : 1;
    Cell *target;

    while(y != dy) {
        ind += dir * width;
        y += dir;
        target = &cells[ind];
        if(!callback(source->el, source, target)) break;
        source = target;
    }
}

void lineMove(U16 x0, U16 y0, I16 x1, I16 y1, _Bool (*callback)(Element *, Cell *, Cell *)) {
    if(!callback || x0 < 0 || x0 >= width || y0 < 0 || y0 >= height) return;
    if(x1 < 0) x1 = 0;
    else if(x1 >= width) x1 = width - 1;
    if(y1 < 0) y1 = 0;
    else if(y1 >= height) y1 = height - 1;
    if(x0 == x1 && y0 == y1) return;

    I16 dx = ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
    I16 dy = -ABS(y1 - y0), sy = y0 < y1 ? 1 : -1; 
    I16 err = dx + dy, e2;

    Cell *source = &cells[y0 * width + x0];
    
    for(;;) {
        e2 = 2 * err;
        if(e2 >= dy) { err += dy; x0 += sx; }
        if(e2 <= dx) { err += dx; y0 += sy; }

        Cell *next = &cells[y0 * width + x0];
        if(!callback(source->el, source, next)) break;
        source = next;
        if(x0 == x1 && y0 == y1) break;
    }
}

void explosionRay(U16 x0, U16 y0, I16 x1, I16 y1, U8 power) {
    if(x1 < 0) x1 = 0;
    else if(x1 >= width) x1 = width - 1;
    if(y1 < 0) y1 = 0;
    else if(y1 >= height) y1 = height - 1;
    if(x0 == x1 && y0 == y1) return;

    I16 dx = ABS(x1 - x0), sx = x0 < x1 ? 1 : -1;
    I16 dy = -ABS(y1 - y0), sy = y0 < y1 ? 1 : -1; 
    I16 err = dx + dy, e2;

    U8 curPower = power;
    
    for(;;) {
        Cell *source = &cells[y0 * width + x0];
        if(curPower) {
            fluid->vx[source->fluidInd] += (x1 - x0) * 0.05;
            fluid->vy[source->fluidInd] += (y1 - y0) * 0.05;
            if(source->el) {
                ElementType type = source->el->type;
                ElementInfo *info = &elementLookup[type];
                U8 resistance = 1;
                switch(info->state) {
                    default :
                        curPower -= resistance;
                        break;
                    case s_UNKNOWN :
                    case s_LIQUID :
                    case s_SOLID :
                        resistance = elementLookup[source->el->type].explosionResistance;
                        if(source->el->scorched) resistance /= 2;
                        if(resistance >= curPower && type != DEBRIS) curPower = 0;
                        else if(type != DEBRIS) curPower -= resistance;
                        if(curPower) {
                            if(info->temp.flammable && type != GUNPOWDER) {
                                source->el->r0 = (randomU8() % info->temp.burnTime) + 1;
                            } else {
                                if(!(type == COBBLESTONE && source->el->r0 == 1) && type != UNBREAKABLECLONER && type != DEBRIS) {
                                    freeCell(source);
                                    if(!info->isStatic && randomBool()) {
                                        spawnElement(source, DEBRIS);
                                        source->el->r0 = type;
                                    }
                                }
                                switch(type) {
                                    case STONE :
                                        if(randEveryU8(4)) {
                                            spawnElement(source, COBBLESTONE);
                                            source->el->r0 = 1;
                                        }
                                        break;
                                    case WATER : spawnElement(source, STEAM); break;
                                    case BATTERY : spawnElement(source, LIGHTNING); break;
                                }
                            }
                        } else source->el->scorched = 1;
                        break;
                }
            } else {
                curPower -= 1;
                if(randEveryU8(power - curPower + 1)) {
                    spawnElement(source, FIRE);
                    source->el->rv %= 30;
                }
            }
        } else {
            if(source->el && elementLookup[source->el->type].state == s_SOLID) source->el->scorched = 1;
            if(randEveryU8(10)) break;
        }

        if(x0 == x1 && y0 == y1) break;
        e2 = 2 * err;
        if(e2 >= dy) { err += dy; x0 += sx; }
        if(e2 <= dx) { err += dx; y0 += sy; }
    }
}

U32 explosionPower = 0;

export U32 getFrameExplosionPower(void) {
    return explosionPower;
}

export void explode(U16 cx, U16 cy, U8 radius) {
    if(cx < 0 || cy < 0 || cx >= width || cy >= height) return;
    explosionPower += radius;
    int x = 0;
	int y = radius;
	int p = 3 - 2 * radius;
	explosionRay(cx, cy, cx + x, cy + y, radius);
	explosionRay(cx, cy, cx + x, cy - y, radius);
	explosionRay(cx, cy, cx - x, cy + y, radius);
	explosionRay(cx, cy, cx - x, cy - y, radius);
	explosionRay(cx, cy, cx + y, cy + x, radius);
	explosionRay(cx, cy, cx + y, cy - x, radius);
	explosionRay(cx, cy, cx - y, cy + x, radius);
	explosionRay(cx, cy, cx - y, cy - x, radius);
	
	while(x < y) {
		if(p < 0) {
			x += 1;
			p = p + 4 * x + 6;
		} else {
			x += 1;
			y -= 1;
			p = p + 4 * (x - y) + 10;
		}
		explosionRay(cx, cy, cx + x, cy + y, radius);
        explosionRay(cx, cy, cx + x, cy - y, radius);
        explosionRay(cx, cy, cx - x, cy + y, radius);
        explosionRay(cx, cy, cx - x, cy - y, radius);
        explosionRay(cx, cy, cx + y, cy + x, radius);
        explosionRay(cx, cy, cx + y, cy - x, radius);
        explosionRay(cx, cy, cx - y, cy + x, radius);
        explosionRay(cx, cy, cx - y, cy - x, radius);
	}
}

void update_liquid(Cell *c) {
    Element *el = c->el;
    ElementInfo *self = &elementLookup[el->type];

    if(!self->attempt(el, c, getCell(c->x, c->y + 1))) {
        moveHorizontally(c, (randomU8() % self->liquid.dispersion + 1) * LORDIR, self->attempt);
    };
}

void update_flammable(Cell *c) {
    Element *el = c->el;
    if(el->r0 >= 1) {
        ElementInfo *self = &elementLookup[el->type];
        if(el->r0 == 2) c->el->scorched = 1;
        if(el->r0 == 1) return freeCell(c);
        el->rv -= 1;
        if(el->rv == 0) el->r0 -= 1;

        APPROACHIFLESS(fluid->density[c->fluidInd], self->temp.burnTemperature, 0.4);

        if(randEveryU8(4)) {
            Cell *target = getCell(c->x + RANDDIR, c->y + RANDDIR);
            if(!target) return;
            else if(!target->el) spawnElement(target, FIRE);
            else {
                ElementInfo *info = &elementLookup[target->el->type];
                if(info->temp.flammable && target->el->r0 == 0) {
                    target->el->r0 = (randomU8() % info->temp.burnTime) + 1;
                }
            }
        }
    }
}

inline void attemptHalt(Element *el) {
    ElementInfo *info = &elementLookup[el->type];
    if(el->sbpy >= info->weight - 1) el->sbpx = el->sbpy * LORDIR * info->solid.impactScatter;
    else el->halted = 1;
}

void tryReleaseHalted(Cell *c) {
    ElementInfo *info = &elementLookup[c->el->type];
    Cell *neighbors[8];
    getMooreNeighborhood(c, neighbors);
    for(U8 i = 0; i < 8; ++i) {
        Cell *check = neighbors[i];
        if(check && check->el && !elementLookup[check->el->type].isStatic && !check->el->halted && randEveryU8(info->solid.inertia)) {
            c->el->halted = 0;
            return;
        }
    }
}

_Bool default_attempt(Element *el, Cell *cell, Cell *target) {
    ElementType targetType = getType(target);
    ElementInfo *targetInfo = &elementLookup[targetType];

    if(getType(target) == EMPTY) {
        moveElement(cell, target);
        return 1;
    } else if(target) switch(targetInfo->state) {
        case s_LIQUID : if(!randEveryU8(4)) return 0;
        case s_GAS :
        case s_PLASMA :
            swapElements(cell, target);
            return 1;
    };
    return 0;
}

void setup_cells() {
    #define ADDENTRY(UPPER,LOWER) elementLookup[UPPER] = LOWER##_info;
    FOREACH_ELEMENTTYPE(ADDENTRY)
    #undef ADDENTRY

    for(U16 i = 0; i < type_length; ++i) {
        if(!elementLookup[i].attempt) elementLookup[i].attempt = default_attempt;
        HSL base = elementLookup[i].baseColor;
        if(base.lig) {
            for(U8 j = 0; j < 8; ++j) {
                elementLookup[i].colors[j] = hslToRgb((HSL){
                    .hue = i == PELLET || i == FIREWORK ? randomF32() : base.hue + (randomF32() * 0.05 - 0.025),
                    .sat = base.sat,
                    .lig = base.lig + (randomF32() * 0.5 - 0.25)
                });
            }
        }
    }
}