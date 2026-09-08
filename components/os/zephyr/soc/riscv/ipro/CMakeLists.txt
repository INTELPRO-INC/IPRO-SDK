# Copyright (c) 2025 IPRO
# SPDX-License-Identifier: Apache-2.0

# Direct subdirectory inclusion based on SOC config
if(CONFIG_SOC_IPRO7)
  add_subdirectory(ipro7)
endif()
