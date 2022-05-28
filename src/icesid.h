#pragma once
#include <memory>
#include <bitset>
#include <queue>

#include "Vicesid.h"
#include <Vicesid___024root.h>

#include "../vcd-writer/vcd_writer.h"

struct icesid {

  icesid(vcd::TimeStamp &time, vcd::VCDWriter *vcd)
    : time       (time)
    , vcd        (vcd)
    , resetCycles(20)
  {
  }

  void init() {
    rtl.reset(new Vicesid);
    rtl->clkEn = 1;

    vcd_var_clk    = vcd->register_var("icesid",      "clk",       vcd::VariableType::wire,     1);
    vcd_var_iWE    = vcd->register_var("icesid",      "iWE",       vcd::VariableType::wire,     1);
    vcd_var_env3   = vcd->register_var("icesid.env3", "oOut",      vcd::VariableType::integer,  8);
    vcd_var_out3   = vcd->register_var("icesid",      "voiceOut2", vcd::VariableType::integer, 12);
    vcd_var_noise3 = vcd->register_var("icesid",      "noise3",    vcd::VariableType::wire,    23);
    vcd_var_phase3 = vcd->register_var("icesid",      "phase3",    vcd::VariableType::wire,    23);
  }

  void step() {

    // toggle clock
    rtl->clk ^= 1;

    // if we are about to step a posedge
    if (rtl->clk == 1) {
      if (!writes.empty() && resetCycles == 0) {
        const auto w = writes.front();
        writes.pop_front();
        rtl->iWE    = 1;
        rtl->iAddr  = w.first;
        rtl->iDataW = w.second;
      }
    }

    rtl->iRst    = resetCycles > 0;
    resetCycles -= resetCycles > 0;

    rtl->eval();

    rtl->iWE = 0;  // deassert write enable

    vcd->change(vcd_var_clk,    time, std::bitset<1>(rtl->clk).to_string());
    vcd->change(vcd_var_iWE,    time, std::bitset<1>(rtl->iWE).to_string());
    vcd->change(vcd_var_env3,   time, std::bitset<8>(rtl->rootp->sid__DOT__env2__DOT__impl__DOT__env).to_string());
    vcd->change(vcd_var_out3,   time, std::bitset<12>(rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__wavMix).to_string());
    vcd->change(vcd_var_noise3, time, std::bitset<23>(rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__lfsr).to_string());
    vcd->change(vcd_var_phase3, time, std::bitset<23>(rtl->rootp->sid__DOT__voices__DOT__voice2__DOT__phase).to_string());

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

  void write_reg(uint8_t addr, uint8_t data) {
    writes.push_back(std::pair<uint8_t, uint8_t>(addr, data));
  }

  void reset() {
    resetCycles = 20;
  }

  void finish() {
  }

  vcd::TimeStamp &time;
  vcd::VCDWriter *vcd;
  std::unique_ptr<Vicesid> rtl;

  vcd::VarPtr vcd_var_clk;
  vcd::VarPtr vcd_var_iWE;
  vcd::VarPtr vcd_var_env3;
  vcd::VarPtr vcd_var_out3;

  vcd::VarPtr vcd_var_noise3;
  vcd::VarPtr vcd_var_phase3;

  std::deque<std::pair<uint8_t, uint8_t>> writes;

  uint32_t resetCycles;
};
