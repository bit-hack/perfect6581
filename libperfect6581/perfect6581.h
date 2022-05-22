#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct state_t state_t;

extern uint32_t transistors;

state_t *initAndResetChip(void);

void     step          (state_t *state);
void     chipStatus    (state_t *state);

void     setRst        (state_t *state, uint32_t);
void     setCs         (state_t *state, uint32_t);
void     setRw         (state_t *state, uint32_t);
void     writeAddress  (state_t *state, uint8_t);
void     writeData     (state_t *state, uint8_t);

uint8_t  readRst       (state_t *state);
uint8_t  readClk       (state_t *state);
uint8_t  readCs        (state_t *state);
uint8_t  readRw        (state_t *state);
uint8_t  readAddress   (state_t *state);
uint8_t  readData      (state_t *state);

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
uint8_t  readCtl3      (state_t *state);  // voice 3 control (8bits)

uint8_t  readEnv3Att   (state_t *state);  // env3 att (4bits)
uint8_t  readEnv3Dec   (state_t *state);  // env3 dec (4bits)
uint8_t  readEnv3Sus   (state_t *state);  // env3 sus (4bits)
uint8_t  readEnv3Rel   (state_t *state);  // env3 rel (4bits)

#ifdef __cplusplus
}  // extern "C"
#endif
