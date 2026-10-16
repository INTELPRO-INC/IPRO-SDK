# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0
#
# Zephyr's samples/bluetooth/bap_broadcast_sink on the IPRO7AI EVK: the
# upstream sources (main, stream_rx, lc3) with the decoded PCM played through
# the GPDAC stream (hw_codec_dac.c) on the on-board speaker instead of the
# upstream I2S codec / USB outputs. Kconfig widened so ENABLE_LC3 is
# selectable on this SoC.
cmake_minimum_required(VERSION 3.20.0)
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
project(bap_broadcast_sink_ipro)

set(UPSTREAM ${ZEPHYR_BASE}/samples/bluetooth/bap_broadcast_sink)
# (no include path to the upstream src: its lc3.h would shadow liblc3's <lc3.h>)
target_compile_definitions(app PRIVATE UPSTREAM_HW_CODEC_H="${UPSTREAM}/src/hw_codec.h")
target_sources(app PRIVATE ${UPSTREAM}/src/main.c ${UPSTREAM}/src/stream_rx.c)
target_sources_ifdef(CONFIG_ENABLE_LC3 app PRIVATE ${UPSTREAM}/src/lc3.c src/hw_codec_dac.c
  src/lc3_plc.c)
# Route the upstream decoder's lc3_decode() through the frame-repeat PLC.
set_source_files_properties(${UPSTREAM}/src/lc3.c PROPERTIES
  COMPILE_DEFINITIONS "lc3_decode=ipro_lc3_decode_plc;lc3_setup_decoder=ipro_lc3_setup_decoder_plc")
