#include <cstdio>
#include <memory>
#include <bitset>

#include "libperfect6581/perfect6581.h"
#include "vcd-writer/vcd_writer.h"


struct mos6581 {

  mos6581()
    : time(0)
  {
  }

  void init() {
    state = initAndResetChip();

    time = 0;

    vcd::HeadPtr header = vcd::makeVCDHeader(vcd::TimeScale::ONE, vcd::TimeScaleUnit::ns, vcd::utils::now());
    vcd.reset(new vcd::VCDWriter{ "dump.vcd", header });
    vcd_var_env3 = vcd->register_var("perfect6581", "env3", vcd::VariableType::integer, 8);
    vcd_var_wav3 = vcd->register_var("perfect6581", "wav3", vcd::VariableType::integer, 12);
  }

  void step() {
    ::step(state);

    uint8_t env3 = readEnvCnt(state);
    vcd->change(vcd_var_env3, time, std::bitset<8>(env3).to_string());

    uint16_t wav3 = readWav3(state);
    vcd->change(vcd_var_wav3, time, std::bitset<12>(wav3).to_string());

    ++time;
  }

  void write_reg(uint8_t addr, uint8_t data) {
    step();
    setCs(state, 0);
    setRw(state, 0);
    writeAddress(state, addr);
    writeData(state, data);
    step();
    setCs(state, 1);
  }

  void finish() {
    vcd->close();
  }

  void *state;
  uint64_t time;

  std::unique_ptr<vcd::VCDWriter> vcd;
  vcd::VarPtr vcd_var_wav3;
  vcd::VarPtr vcd_var_env3;
};

int main(int argc, char **args) {

    mos6581 sid;

    sid.init();

    if (argc <= 1) {
      return 1;
    }

    FILE *fd = fopen(args[1], "r");
    if (!fd) {
      return 1;
    }

    while (!feof(fd)) {

      uint32_t cycles = 0;
      uint32_t reg    = 0;
      uint32_t val    = 0;
      if (3 != fscanf(fd, "%x %x %x", &cycles, &reg, &val)) {
        break;
      }

      if (reg <= 0x1c && val <= 0xff) {
        sid.write_reg(reg, val);
      }

      for (int i = 0; i < cycles; ++i) {
        sid.step();
        sid.step();
      }
    }

    if (fd) {
      fclose(fd);
    }

    sid.finish();
}
