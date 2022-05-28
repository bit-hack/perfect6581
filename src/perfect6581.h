#pragma once

#include <cstdint>

#include "../libperfect6581/perfect6581.h"
#include "vcd.h"

struct perfect6581_t {

  perfect6581_t(vcd::vcd_t &vcd)
    : state(nullptr)
    , vcd(vcd)
    , resetCycles(20)
  {
  }

  void init() {
    state = initAndResetChip();

    vcd.module_begin("perfect6581");
    vcd.def(vcd_var_clk        , "p6_clk",    vcd::var_type_wire,    1);
    vcd.def(vcd_var_rst        , "p6_rst",    vcd::var_type_wire,    1);
    vcd.def(vcd_var_cs         , "p6_cs",     vcd::var_type_wire,    1);
    vcd.def(vcd_var_rw         , "p6_rw",     vcd::var_type_wire,    1);
    vcd.def(vcd_var_addr       , "p6_addr",   vcd::var_type_wire,    5);
    vcd.def(vcd_var_data       , "p6_data",   vcd::var_type_wire,    8);
    vcd.def(vcd_var_env3       , "p6_env3",   vcd::var_type_integer, 8);
    vcd.def(vcd_var_wav3       , "p6_wav3",   vcd::var_type_integer, 12);
    vcd.def(vcd_var_ctl3_gate  , "p6_gate",   vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_sync  , "p6_sync",   vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_xor   , "p6_xor",    vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_test  , "p6_test",   vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_tri   , "p6_tri",    vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_saw   , "p6_saw",    vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_pulse , "p6_pulse",  vcd::var_type_wire,    1);
    vcd.def(vcd_var_ctl3_noise , "p6_noise",  vcd::var_type_wire,    1);
    vcd.def(vcd_var_env3_att   , "p6_att",    vcd::var_type_integer, 4);
    vcd.def(vcd_var_env3_dec   , "p6_dec",    vcd::var_type_integer, 4);
    vcd.def(vcd_var_env3_sus   , "p6_sus",    vcd::var_type_integer, 4);
    vcd.def(vcd_var_env3_rel   , "p6_rel",    vcd::var_type_integer, 4);
    vcd.def(vcd_var_noise3     , "p6_noise3", vcd::var_type_wire,    23);
    vcd.def(vcd_var_phase3     , "p6_phase3", vcd::var_type_wire,    23);
    vcd.module_end();
  }

  void step() {

    // if we are about to step a posedge
    if (readClk(state) == 0) {
      if (!writes.empty() && resetCycles == 0) {
        const auto w = writes.front();
        writes.pop_front();
        setCs(state, 0);
        setRw(state, 0);
        writeAddress(state, w.first);
        writeData(state, w.second);
      }
    }

    // handle reset
    setRst(state, (resetCycles > 0) ? 0 : 1);
    resetCycles -= resetCycles > 0;

    ::step(state);

    setCs(state, 1);  // deassert CS

    vcd.set(vcd_var_clk,    readClk    (state));
    vcd.set(vcd_var_rst,    readRst    (state));
    vcd.set(vcd_var_cs,     readCs     (state));
    vcd.set(vcd_var_rw,     readRw     (state));
    vcd.set(vcd_var_addr,   readAddress(state));
    vcd.set(vcd_var_data,   readData   (state));
    vcd.set(vcd_var_phase3, readAcc3   (state));
    vcd.set(vcd_var_env3,   readEnvCnt (state));
    vcd.set(vcd_var_wav3,   readWav3   (state));

    uint16_t ctl3 = readCtl3(state);
    vcd.set(vcd_var_ctl3_gate,  (ctl3 & (1 << 0)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_sync,  (ctl3 & (1 << 1)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_xor,   (ctl3 & (1 << 2)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_test,  (ctl3 & (1 << 3)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_tri,   (ctl3 & (1 << 4)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_saw,   (ctl3 & (1 << 5)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_pulse, (ctl3 & (1 << 6)) ? 1 : 0);
    vcd.set(vcd_var_ctl3_noise, (ctl3 & (1 << 7)) ? 1 : 0);

    vcd.set(vcd_var_env3_att,   readEnv3Att(state));
    vcd.set(vcd_var_env3_dec,   readEnv3Dec(state));
    vcd.set(vcd_var_env3_sus,   readEnv3Sus(state));
    vcd.set(vcd_var_env3_rel,   readEnv3Rel(state));
    vcd.set(vcd_var_noise3,     readNoi3(state));
  }

  void write_reg(uint8_t addr, uint8_t data) {
    writes.push_back(std::pair<uint8_t, uint8_t>(addr, data));
  }

  void reset() {
    resetCycles = 20;
  }

  void finish() {
  }

  state_t *state;

  vcd::vcd_t &vcd;

  vcd::var_t vcd_var_noise3;
  vcd::var_t vcd_var_clk;
  vcd::var_t vcd_var_rst;
  vcd::var_t vcd_var_cs;
  vcd::var_t vcd_var_rw;
  vcd::var_t vcd_var_addr;
  vcd::var_t vcd_var_data;
  vcd::var_t vcd_var_wav3;
  vcd::var_t vcd_var_env3;
  vcd::var_t vcd_var_ctl3_gate;
  vcd::var_t vcd_var_ctl3_sync;
  vcd::var_t vcd_var_ctl3_xor;
  vcd::var_t vcd_var_ctl3_test;
  vcd::var_t vcd_var_ctl3_tri;
  vcd::var_t vcd_var_ctl3_saw;
  vcd::var_t vcd_var_ctl3_pulse;
  vcd::var_t vcd_var_ctl3_noise;
  vcd::var_t vcd_var_env3_att;
  vcd::var_t vcd_var_env3_dec;
  vcd::var_t vcd_var_env3_sus;
  vcd::var_t vcd_var_env3_rel;
  vcd::var_t vcd_var_phase3;

  std::deque<std::pair<uint8_t, uint8_t>> writes;

  uint32_t resetCycles;
};
