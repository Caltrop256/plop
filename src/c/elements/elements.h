#ifndef ELEMENTS_H
#define ELEMENTS_H

#include "../color.h"

#define FOREACH_ELEMENTTYPE(MACRO) \
    MACRO(SAND,sand)\
    MACRO(STONE,stone)\
    MACRO(COBBLESTONE,cobblestone)\
    MACRO(GUNPOWDER,gunpowder)\
    MACRO(WOOD,wood)\
    MACRO(SNOW,snow)\
    MACRO(ICE,ice)\
    MACRO(SEED,seed)\
    MACRO(PLANT,plant)\
    MACRO(PELLET,pellet)\
    MACRO(DIRT,dirt)\
    MACRO(COAL,coal)\
    MACRO(GLASS,glass)\
    MACRO(TITANIUM,titanium)\
    MACRO(BATTERY,battery)\
    MACRO(COPPER,copper)\
    MACRO(WATER,water)\
    MACRO(OIL,oil)\
    MACRO(CEMENT,cement)\
    MACRO(ACID,acid)\
    MACRO(MAGMA,magma)\
    MACRO(STEAM,steam)\
    MACRO(TOXIC,toxic)\
    MACRO(SMOKE,smoke)\
    MACRO(FIRE,fire)\
    MACRO(LIGHTNING,lightning)\
    MACRO(CLONER,cloner)\
    MACRO(UNBREAKABLECLONER,unbreakablecloner)\
    MACRO(NEUTRON,neutron)\
    MACRO(ELECTRON,electron)\
    MACRO(PHOTON,photon)\
    MACRO(NITRO,nitro)\
    MACRO(FUSE,fuse)\
    MACRO(PROTON,proton)\
    MACRO(TORCH,torch)\
    MACRO(DEBRIS,debris)\
    MACRO(LIQUIDNITROGEN,liquidnitrogen)\
    MACRO(FOG,fog)\
    MACRO(THERMITE,thermite)\
    MACRO(SPARK,spark)\
    MACRO(POLONIUM,polonium)\
    MACRO(PLUTONIUM,plutonium)\
    MACRO(PUMP,pump)\
    MACRO(CONVEYER,conveyer)\
    MACRO(PISTON,piston)\
    MACRO(FUNGUS,fungus)\
    MACRO(FIREWORK,firework)\
    MACRO(DYNAMITE,dynamite)

typedef enum ElementType {
    VOID,
    EMPTY,
    #define GENERATE_ENUM(ENUM,_) ENUM,
    FOREACH_ELEMENTTYPE(GENERATE_ENUM)
    #undef GENERATE_ENUM
    type_length
} ElementType;

typedef struct Cell Cell;

typedef struct Element {
    ElementType type;
    U8 rv;
    U8 r0;
    U8 tick;

    // Element Colors
    U8 color : 3;
    U8 scorched : 1;
    U8 halted : 1;
    U8 electricityState : 4;

    Cell *cell;

    float sbpx;
    float sbpy;
} Element;


typedef struct Cell {
    Element *el;
    U16 x;
    U16 y;
    U32 cellInd;
    F32 temperature;
    U32 fluidInd;
    U32 fluidNodes[4];
    F32 fluidNodeWeights[4];
} Cell;

typedef enum MatterState {
    s_UNKNOWN,
    s_SOLID,
    s_LIQUID,
    s_GAS,
    s_PLASMA
} MatterState;

typedef enum ElementAction {
    a_NONE,
    a_MOVE,
    a_SWAP
} ElementAction;

typedef struct SolidInfo {
    U8 inertia;
    F32 impactScatter;
} SolidInfo;

typedef struct LiquidInfo {
    U8 dispersion;
    U8 density;
} LiquidInfo;

typedef struct TemperatureInfo {
    _Bool flammable;
    U8 burnTime;
    U16 burnTemperature;
    F32 insolation;
} TemperatureInfo;

typedef struct ElementInfo {
    void (*handler)(Element *, Cell *, U16, U16);
    _Bool (*attempt)(Element *, Cell *, Cell *);
    void (*init)(Element *el);

    HSL baseColor;
    Color colors[8];

    _Bool isStatic;
    _Bool dontLimitSubPixelMovementSpeed;
    _Bool conductive;
    F32 weight;
    U8 explosionResistance;

    MatterState state;
    SolidInfo solid;
    LiquidInfo liquid;

    TemperatureInfo temp;
} ElementInfo;

extern ElementInfo fire_info;

#define RANDDIR (randomU8() % 3 - 1)
#define LORDIR (randomBool() ? -1 : 1)

void freeCell(Cell *c);
void spawnElement(Cell *cell, ElementType type);
void getMooreNeighborhood(Cell *c, Cell **out);
Cell *getCell(I32 x, I32 y);
ElementType getType(Cell *c);
void moveElement(Cell *source, Cell *target);
void swapElements(Cell *source, Cell *target);
void moveHorizontally(Cell *source, I16 distance, _Bool (*callback)(Element *, Cell *, Cell *));
void moveVertically(Cell *source, I16 distance, _Bool (*callback)(Element *, Cell *, Cell *));
void lineMove(U16 x0, U16 y0, I16 x1, I16 y1, _Bool (*callback)(Element *, Cell *, Cell *));
void explode(U16 cx, U16 cy, U8 radius);
void update_liquid(Cell *c);
void update_flammable(Cell *c);
void attemptHalt(Element *el);
void tryReleaseHalted(Cell *c);
_Bool default_attempt(Element *el, Cell *cell, Cell *target);

void setup_cells();

extern _Bool voidDelete;
extern U8 g_tick;
extern Cell *cells;
extern U16 width;
extern U16 height;
extern ElementInfo elementLookup[type_length];
extern U32 explosionPower;

#define GENERATE_EXTERN_ELEMENTINFO(UPPER, LOWER) extern ElementInfo LOWER##_info;
FOREACH_ELEMENTTYPE(GENERATE_EXTERN_ELEMENTINFO)
#undef GENERATE_EXTERN_ELEMENTINFO

#define VARJOINERINNER(A, B) A ## _ ## B
#define VARJOINER(A, B) VARJOINERINNER(A, B)
#define EXPAND_MACRO(x) x
#define VARPREF(PREFIX) VARJOINER(PREFIX, EXPAND_MACRO(ELEMENT_NAME))
#define VARPOST(POSTFIX) VARJOINER(EXPAND_MACRO(ELEMENT_NAME), POSTFIX)

#endif
