#include "icesid.h"

icesid_t::icesid_t(vcd::vcd_t &vcd)
  : vcd(vcd)
  , resetCycles(20)
{
}

void icesid_t::init() {
  rtl.reset(new Vicesid);
  rtl->clkEn = 1;

  vcd.module_begin("icesid");
  vcd.def(vcd_var_clk,    "is_clk",       vcd::var_type_wire,    1);
  vcd.def(vcd_var_iWE,    "is_iWE",       vcd::var_type_wire,    1);
  vcd.def(vcd_var_env3,   "is_oOut",      vcd::var_type_integer, 8);
  vcd.def(vcd_var_out3,   "is_voiceOut2", vcd::var_type_integer, 12);
  vcd.def(vcd_var_noise3, "is_noise3",    vcd::var_type_wire,    23);
  vcd.def(vcd_var_phase3, "is_phase3",    vcd::var_type_wire,    23);
  vcd.module_end();
}

void icesid_t::step() {

  // toggle clock
  rtl->clk ^= 1;

  // if we are about to step a posedge
  if (rtl->clk == 1) {
    if (!writes.empty() && resetCycles == 0) {
      const auto w = writes.front();
      writes.pop_front();
      rtl->iWE = 1;
      rtl->iAddr = w.first;
      rtl->iDataW = w.second;
    }
  }

  rtl->iRst = resetCycles > 0;
  resetCycles -= resetCycles > 0;

  rtl->eval();

  vcd.set(vcd_var_clk,    rtl->clk);
  vcd.set(vcd_var_iWE,    rtl->iWE);
  vcd.set(vcd_var_env3,   rtl->rootp->sid__DOT__env2__DOT__impl__DOT__env);
  vcd.set(vcd_var_out3,   rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__wavMix);
  vcd.set(vcd_var_noise3, rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__lfsr);
  vcd.set(vcd_var_phase3, rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__phase);

  rtl->iWE = 0;  // deassert write enable

  // perform non clock enable cycles
  if (rtl->clk == 0) {
    rtl->clkEn = 0;
    for (uint32_t i = 0; i < 10; ++i) {
      rtl->clk ^= 1;
      rtl->eval();
    }
    rtl->clkEn = 1;
  }

}

void icesid_t::write_reg(uint8_t addr, uint8_t data) {
  writes.push_back(std::pair<uint8_t, uint8_t>(addr, data));
}

void icesid_t::reset() {
  resetCycles = 20;
}

void icesid_t::finish() {
}
