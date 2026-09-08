# IPRO7 Board CMake Extensions
# Copyright (c) 2025 IPRO
# SPDX-License-Identifier: Apache-2.0

# Add board-specific linker flags to suppress RWX warnings
# This is common in embedded systems where code and data may be mixed
add_link_options(-Wl,--no-warn-rwx-segments)
