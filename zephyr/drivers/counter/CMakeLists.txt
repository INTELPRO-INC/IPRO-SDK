# Copyright (c) 2026 INTELPRO
# SPDX-License-Identifier: Apache-2.0

zephyr_library()
zephyr_library_sources_ifdef(CONFIG_COUNTER_IPRO counter_ipro.c)
zephyr_library_sources_ifdef(CONFIG_COUNTER_IPRO_RTC counter_ipro_rtc.c)
