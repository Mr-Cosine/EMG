#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#if defined(ARDUINO_ARCH_AVR) 
typedef int16_t integer;
#else 
typedef int32_t integer;
#endif