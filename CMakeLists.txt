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

add_library(
  libicesid
  libicesid/icesid.h
  libicesid/icesid.cpp)

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator not found. Set VERILATOR_ROOT env var.")
endif()

file(GLOB ICE_SRC libicesid/*.v)
verilate(libicesid
  SOURCES ${ICE_SRC}
  TOP_MODULE sid
  PREFIX Vicesid
  TRACE
)

add_executable(
  test
  test.cpp
)

target_link_libraries(
  test
  libperfect6581
  libicesid
  libvcd-writer
)
