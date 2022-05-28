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

find_package(verilator HINTS $ENV{VERILATOR_ROOT} ${VERILATOR_ROOT})
if (NOT verilator_FOUND)
  message(FATAL_ERROR "Verilator not found. Set VERILATOR_ROOT env var.")
endif()

add_executable(
  driver
  src/driver.cpp
  src/perfect6581.h
)

add_library(
  libicesid
  src/icesid.h
  src/icesid.cpp
)

file(GLOB ICE_SRC icesid/icesid/*.v icesid/verilator/*.v)
verilate(libicesid
  SOURCES ${ICE_SRC}
  TOP_MODULE sid
  PREFIX Vicesid
  TRACE
)

target_link_libraries(
  driver
  libperfect6581
  libicesid
)
