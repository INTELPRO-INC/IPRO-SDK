# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources(dac_ipro.c)
zephyr_library_sources_ifdef(CONFIG_DAC_IPRO_STREAM dac_ipro_stream.c)
