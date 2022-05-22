#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct state_t state_t;

state_t *setupNodesAndTransistors(
  netlist_transdefs *transdefs,
  bool *node_is_pullup,
  nodenum_t nodes,
  nodenum_t transistors,
  nodenum_t vss,
  nodenum_t vcc);

void setNode(
  state_t *state,
  nodenum_t nn,
  bool s);

bool isNodeHigh(
  state_t *state,
  nodenum_t nn);

uint32_t readNodes(
  state_t *state,
  int count,
  nodenum_t *nodelist);

void writeNodes(
  state_t *state,
  int count,
  nodenum_t *nodelist,
  int v);

void recalcNodeList(
  state_t *state);

void stabilizeChip(
  state_t *state);

#ifdef __cplusplus
}  // extern "C"
#endif
