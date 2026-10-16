#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

// Fixed widths matching the ESP32 target, so PC test builds give the same results:
// int and long are 32-bit integers, and "double" precision is a 32-bit float
// (the ESP32 FPU is single-precision; 64-bit doubles are emulated in software).
typedef int32_t i32;
typedef float   f32;

static_assert(sizeof(i32) == 4, "i32 must be 32-bit");
static_assert(sizeof(f32) == 4, "f32 must be 32-bit");

#endif
