#pragma once

#include <stdint.h>

typedef uint32_t bool;
typedef uint16_t nodenum_t;

typedef struct {
	int32_t gate;
	int32_t c1;
	int32_t c2;
} netlist_transdefs;

enum {
   YES = 1,
   NO  = 0
};
