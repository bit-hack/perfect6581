#define _CRT_SECURE_NO_WARNINGS
#include <cstdio>
#include <memory>
#include <bitset>
#include <deque>

#include "vcd.h"
#include "icesid.h"
#include "perfect6581.h"

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

    uint64_t time = 0;

    vcd::vcd_t vcd;
    vcd.begin(vcd_path, 1, vcd::timescale_ps);

    perfect6581_t psid{ vcd };
    icesid_t      isid{ vcd };

    vcd.defs_begin();
    psid.init();
    isid.init();
    vcd.defs_end();

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

        vcd.timestamp(time);
        psid.step();
        isid.step();
        ++time;

        vcd.timestamp(time);
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

    vcd.end();

    return 0;
}
