# Minimal CMake configuration for QEMU CLI testing
cmake_minimum_required(VERSION 3.20.0)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(minimal_qemu_cli)

target_sources(app PRIVATE main.c)
