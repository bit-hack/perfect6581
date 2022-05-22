#include <cstdio>
#include <memory>
#include <bitset>
#include <deque>

#include "libperfect6581/perfect6581.h"
#include "vcd-writer/vcd_writer.h"
#include "libicesid/icesid.h"

// voice3:
//
//   0e  ffffffff  f-frequency low byte
//   0f  ffffffff  f-frequency high byte
//   10  pppppppp  p-pulse wave duty cycle low
//   11  ....pppp  p-pulse wave duty cycle high
//
//   12  npstTRSG  
//       |||||||'- G-gate       01
//       ||||||'-- S-sync       02
//       |||||'--- R-ring       04
//       ||||'---- T-test       08
//       |||'----- t-triangle   10
//       ||'------ s-saw        20
//       |'------- p-pulse      40
//       '-------- n-noise      80
//
//   13  aaaadddd  a-attack d-decay
//   14  ssssrrrr  s-sustain r-release
//
//   1b  oooooooo  o-oscillator voice 3
//   1c  eeeeeeee  e-envelope voice 3


struct perfect6581 {

  perfect6581(vcd::TimeStamp &time, vcd::VCDWriter *vcd)
    : state      (nullptr)
    , time       (time)
    , vcd        (vcd)
    , resetCycles(20)
  {
  }

  void init() {
    state = initAndResetChip();

    time = 0;

    vcd_var_clk        = vcd->register_var("perfect6581",      "clk",   vcd::VariableType::wire,      1);
    vcd_var_rst        = vcd->register_var("perfect6581",      "rst",   vcd::VariableType::wire,      1);
    vcd_var_cs         = vcd->register_var("perfect6581",      "cs",    vcd::VariableType::wire,      1);
    vcd_var_rw         = vcd->register_var("perfect6581",      "rw",    vcd::VariableType::wire,      1);
    vcd_var_addr       = vcd->register_var("perfect6581",      "addr",  vcd::VariableType::wire,      5);
    vcd_var_data       = vcd->register_var("perfect6581",      "data",  vcd::VariableType::wire,      8);
    vcd_var_env3       = vcd->register_var("perfect6581",      "env3",  vcd::VariableType::integer,   8);
    vcd_var_wav3       = vcd->register_var("perfect6581",      "wav3",  vcd::VariableType::integer,  12);
    vcd_var_ctl3_gate  = vcd->register_var("perfect6581.ctl3", "gate",  vcd::VariableType::wire,      1);
    vcd_var_ctl3_sync  = vcd->register_var("perfect6581.ctl3", "sync",  vcd::VariableType::wire,      1);
    vcd_var_ctl3_xor   = vcd->register_var("perfect6581.ctl3", "xor",   vcd::VariableType::wire,      1);
    vcd_var_ctl3_test  = vcd->register_var("perfect6581.ctl3", "test",  vcd::VariableType::wire,      1);
    vcd_var_ctl3_tri   = vcd->register_var("perfect6581.ctl3", "tri",   vcd::VariableType::wire,      1);
    vcd_var_ctl3_saw   = vcd->register_var("perfect6581.ctl3", "saw",   vcd::VariableType::wire,      1);
    vcd_var_ctl3_pulse = vcd->register_var("perfect6581.ctl3", "pulse", vcd::VariableType::wire,      1);
    vcd_var_ctl3_noise = vcd->register_var("perfect6581.ctl3", "noise", vcd::VariableType::wire,      1);
    vcd_var_env3_att   = vcd->register_var("perfect6581.env3", "att",   vcd::VariableType::integer,   4);
    vcd_var_env3_dec   = vcd->register_var("perfect6581.env3", "dec",   vcd::VariableType::integer,   4);
    vcd_var_env3_sus   = vcd->register_var("perfect6581.env3", "sus",   vcd::VariableType::integer,   4);
    vcd_var_env3_rel   = vcd->register_var("perfect6581.env3", "rel",   vcd::VariableType::integer,   4);
  }

  void step() {

    // if we are about to step a posedge
    if (readClk(state) == 0) {
      if (!writes.empty() && resetCycles == 0) {
        const auto w = writes.front();
        writes.pop_front();
        setCs       (state, 0);
        setRw       (state, 0);
        writeAddress(state, w.first);
        writeData   (state, w.second);
      }
    }

    // handle reset
    setRst(state, (resetCycles > 0) ? 0 : 1);
    resetCycles -= resetCycles > 0;

    ::step(state);

    setCs(state, 1);  // deassert CS

    vcd->change(vcd_var_clk,  time, std::bitset<1>(readClk(state)).to_string());
    vcd->change(vcd_var_rst,  time, std::bitset<1>(readRst(state)).to_string());
    vcd->change(vcd_var_cs,   time, std::bitset<1>(readCs(state)).to_string());
    vcd->change(vcd_var_rw,   time, std::bitset<1>(readRw(state)).to_string());
    vcd->change(vcd_var_addr, time, std::bitset<5>(readAddress(state)).to_string());
    vcd->change(vcd_var_data, time, std::bitset<8>(readData(state)).to_string());

    uint8_t env3 = readEnvCnt(state);
    vcd->change(vcd_var_env3, time, std::bitset<8>(env3).to_string());

    uint16_t wav3 = readWav3(state);
    vcd->change(vcd_var_wav3, time, std::bitset<12>(wav3).to_string());

    uint16_t ctl3 = readCtl3(state);
    vcd->change(vcd_var_ctl3_gate,  time, (ctl3 & (1 << 0)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_sync,  time, (ctl3 & (1 << 1)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_xor,   time, (ctl3 & (1 << 2)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_test,  time, (ctl3 & (1 << 3)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_tri,   time, (ctl3 & (1 << 4)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_saw,   time, (ctl3 & (1 << 5)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_pulse, time, (ctl3 & (1 << 6)) ? "1" : "0");
    vcd->change(vcd_var_ctl3_noise, time, (ctl3 & (1 << 7)) ? "1" : "0");

    vcd->change(vcd_var_env3_att, time, std::bitset<4>(readEnv3Att(state)).to_string());
    vcd->change(vcd_var_env3_dec, time, std::bitset<4>(readEnv3Dec(state)).to_string());
    vcd->change(vcd_var_env3_sus, time, std::bitset<4>(readEnv3Sus(state)).to_string());
    vcd->change(vcd_var_env3_rel, time, std::bitset<4>(readEnv3Rel(state)).to_string());
  }

  void write_reg(uint8_t addr, uint8_t data) {
    writes.push_back(std::pair<uint8_t, uint8_t>(addr, data));
  }

  void reset() {
    resetCycles = 20;
  }

  void finish() {
    vcd->close();
  }

  vcd::TimeStamp &time;
  state_t *state;

  vcd::VCDWriter *vcd;

  vcd::VarPtr vcd_var_clk;
  vcd::VarPtr vcd_var_rst;
  vcd::VarPtr vcd_var_cs;
  vcd::VarPtr vcd_var_rw;
  vcd::VarPtr vcd_var_addr;
  vcd::VarPtr vcd_var_data;
  vcd::VarPtr vcd_var_wav3;
  vcd::VarPtr vcd_var_env3;
  vcd::VarPtr vcd_var_ctl3_gate;
  vcd::VarPtr vcd_var_ctl3_sync;
  vcd::VarPtr vcd_var_ctl3_xor;
  vcd::VarPtr vcd_var_ctl3_test;
  vcd::VarPtr vcd_var_ctl3_tri;
  vcd::VarPtr vcd_var_ctl3_saw;
  vcd::VarPtr vcd_var_ctl3_pulse;
  vcd::VarPtr vcd_var_ctl3_noise;
  vcd::VarPtr vcd_var_env3_att;
  vcd::VarPtr vcd_var_env3_dec;
  vcd::VarPtr vcd_var_env3_sus;
  vcd::VarPtr vcd_var_env3_rel;

  std::deque<std::pair<uint8_t, uint8_t>> writes;

  uint32_t resetCycles;
};

int main(int argc, char **args) {

    if (argc <= 1) {
      fprintf(stderr, "usage: %s input.txt [output.vcd]", args[0]);
      return 1;
    }

    FILE *fd = fopen(args[1], "r");
    if (!fd) {
      fprintf(stderr, "unable to open %s", args[1]);
      return 1;
    }

    const char *vcd_path = argc >= 3 ? args[2] : "output.vcd";


    vcd::HeadPtr header = vcd::makeVCDHeader(
      vcd::TimeScale::ONE,
      vcd::TimeScaleUnit::ns,
      vcd::utils::now());
    vcd::VCDWriter vcd{ vcd_path, header };

    vcd::TimeStamp time = 0;

    perfect6581 psid{ time, &vcd };
    icesid      isid{ time, &vcd };

    psid.init();
    isid.init();

    while (!feof(fd)) {

      static char dummy[1024];

      uint32_t cycles = 0;
      uint32_t reg    = 0;
      uint32_t val    = 0;
      if (fscanf(fd, "%x %x %x%[^\n]\n", &cycles, &reg, &val, dummy) < 3) {
        break;
      }

      if (reg == 0xfe) {
        psid.reset();
        isid.reset();
      }

      if (reg <= 0x1c && val <= 0xff) {
        psid.write_reg(reg, val);
        isid.write_reg(reg, val);
      }

      for (uint32_t i = 0; i < cycles; ++i) {
        psid.step();
        isid.step();
        ++time;
        psid.step();
        isid.step();
        ++time;
      }
    }

    if (fd) {
      fclose(fd);
    }

    psid.finish();
    isid.finish();
}
