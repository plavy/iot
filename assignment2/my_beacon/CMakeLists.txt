cmake_minimum_required(VERSION 3.20.0)

set(BOARD "thingy52_nrf52832")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(assignment2)

FILE(GLOB app_sources
      src/main.c
      src/led.c
      src/hts.c)
target_sources(app PRIVATE
  ${app_sources}
  )

zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)
