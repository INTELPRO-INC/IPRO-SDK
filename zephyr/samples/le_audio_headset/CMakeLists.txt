# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# LE Audio unicast headset on the IPRO7AI EVK: CAP acceptor (CAS + CSIP set
# member), PACS/ASCS sink, VCP volume renderer; a phone's LE Audio unicast
# client (Android "LE Audio" headphones) streams LC3 over a CIS, decoded and
# played on the GPDAC speaker. Port of apps/bluetooth/le_audio/ipro_le_audio_headset.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(le_audio_headset_ipro)

target_include_directories(app PRIVATE ../common/audio)
target_sources(app PRIVATE src/main.c src/bap_server.c src/audio.c ../common/audio/pcm_out.c)
