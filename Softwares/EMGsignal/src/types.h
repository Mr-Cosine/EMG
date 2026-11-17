#ifndef TYPES_H
#define TYPES_H

#include <stdint.h>

#if defined(ARDUINO_ARCH_AVR) 
typedef int16_t _int;
#else 
typedef int32_t _int;
#endif

typedef int8_t _short;
typedef float _float;