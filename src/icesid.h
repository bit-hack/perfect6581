#pragma once
#include <memory>
#include <bitset>
#include <queue>

#include "Vicesid.h"
#include <Vicesid___024root.h>

#include "vcd.h"

struct icesid_t {

  icesid_t(vcd::vcd_t &vcd);

  void init();

  void step();

  void write_reg(uint8_t addr, uint8_t data);

  void reset();

  void finish();

  vcd::vcd_t &vcd;
  std::unique_ptr<Vicesid> rtl;

  vcd::var_t vcd_var_clk;
  vcd::var_t vcd_var_iWE;
  vcd::var_t vcd_var_env3;
  vcd::var_t vcd_var_out3;
  vcd::var_t vcd_var_noise3;
  vcd::var_t vcd_var_phase3;

  std::deque<std::pair<uint8_t, uint8_t>> writes;

  uint32_t resetCycles;
};
