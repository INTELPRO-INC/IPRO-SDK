# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# BLE OTA on IPRO7: MCUboot (sysbuild, swap-move) + MCUmgr SMP over Bluetooth
# LE (image + OS management). Upload a signed image with nRF Connect Device
# Manager / AuTerm / the mcumgr CLI, mark it for test, reset: MCUboot swaps
# it in, the new image confirms itself on its first boot.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(ipro_ble_ota)
target_sources(app PRIVATE src/main.c)
