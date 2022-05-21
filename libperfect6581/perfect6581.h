#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef INCLUDED_FROM_NETLIST_SIM_C
#  define state_t void
#endif

extern uint32_t cycle;
extern uint32_t transistors;

extern state_t *initAndResetChip();
extern void    step        (state_t *state);
extern void    chipStatus  (state_t *state);
extern void    setCs       (state_t *state, uint32_t);
extern void    setRw       (state_t *state, uint32_t);
extern void    writeAddress(state_t *state, uint8_t);
extern void    writeData   (state_t *state, uint8_t);
extern uint8_t readData    (state_t *state);

uint8_t  readDataBus   (state_t *state);
uint8_t  readAddressBus(state_t *state);
uint16_t readLfsr15    (state_t *state);  // env 3 lfsr 15
uint8_t  readLfsr5     (state_t *state);  // env 3 lfsr 5
uint8_t  readEnvCnt    (state_t *state);  // env 3 (8 bit)
uint32_t readAcc3      (state_t *state);  // voice 3 accumulator (24bit)
uint16_t readTri3      (state_t *state);  // voice 3 triangle (16bit)
bool     readPwm3      (state_t *state);  // voice 3 pwm
bool     readTriXor3   (state_t *state);  // voice 3 ring-mod
uint32_t readNoi3      (state_t *state);  // voice 3 noise
uint16_t readWav3      (state_t *state);  // voice 3 output (12bits)

#ifdef __cplusplus
}  // extern "C"
#endif
