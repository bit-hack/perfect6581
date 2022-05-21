cmake_minimum_required(VERSION 3.0)
project(perfect6581)

add_library(
  libperfect6581
  libperfect6581/perfect6581
  libperfect6581/netlist_6581.h
  libperfect6581/netlist_sim.c
  libperfect6581/netlist_sim.h
  libperfect6581/perfect6581.c
  libperfect6581/perfect6581.h
  libperfect6581/types.h
)

add_library(
  libvcd-writer
  vcd-writer/vcd_utils.cpp
  vcd-writer/vcd_writer.cpp
  vcd-writer/vcd_writer.h
)

add_executable(
  test
  test.cpp
)

target_link_libraries(
  test
  libperfect6581
  libvcd-writer
)
