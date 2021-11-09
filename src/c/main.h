#ifndef TYPEDEFS_H
#define TYPEDEFS_H
#include <stdint.h>
typedef uint8_t     U8;
typedef int8_t      I8;
typedef uint16_t    U16;
typedef int16_t     I16;
typedef uint32_t    U32;
typedef int32_t     I32;
typedef float       F32;
typedef double      F64;

#define export __attribute__((visibility("default")))

#define MAX(x, y) ((x) >= (y) ? (x) : (y))
#define MIN(x, y) ((x) <= (y) ? (x) : (y))
#define CLAMP(val, min, max) MIN(max, MAX(min, val))
#define SIGN(x) ((x) > 0 ? (1) : (x) < 0 ? (-1) : (0))
#define ABS(x) ((x) < 0 ? (-(x)) : x)
#define APPROACH(source, target, slope) ((source) = ((source) - (target)) * (slope) + (target))
#define APPROACHIFLESS(source, target, slope) if(source < target) ((source) = ((source) - (target)) * (slope) + (target))
#define APPROACHIFMORE(source, target, slope) if(source > target) ((source) = ((source) - (target)) * (slope) + (target))

#ifndef NULL
#define NULL ((void *)0)
#endif

#define RANDDIR (randomU8() % 3 - 1)
#define LORDIR (randomBool() ? -1 : 1)

// imported functions
__attribute__((import_module("env"), import_name("log"))) void env_log();
__attribute__((import_module("env"), import_name("cos"))) double env_cos(double);
__attribute__((import_module("env"), import_name("sin"))) double env_sin(double);
__attribute__((import_module("env"), import_name("atan2"))) double env_atan2(double, double);

#endif
