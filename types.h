#ifndef _TYPES_H_
#define _TYPES_H_

#include <stdint.h>

typedef uint32_t bool;
typedef uint16_t nodenum_t;

typedef struct {
	int gate;
	int c1;
	int c2;
} netlist_transdefs;

#define YES 1
#define NO 0

#endif