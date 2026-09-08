SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= fpga
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_throughput_test
# add custom cmake definition
#cmake_definition+=-Dxxx=sss

# include $(IPRO_SDK_BASE)/project.build

#
# Default target
#
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

#
# Configuration Management
#
CONFIG_FILE = $(PROJECT_PATH)/.config

# Show current configuration
show-config:
	@if [ -f "$(CONFIG_FILE)" ]; then \
		echo "Current configuration (.config):"; \
		cat $(CONFIG_FILE); \
	else \
		echo "Error: .config not found"; \
	fi

#
# Build Targets
#
clean: clean-build

clean-build:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) clean

clean-config:
	@rm -f $(CONFIG_FILE) $(CONFIG_FILE).old
	@echo "Configuration files cleaned"

distclean: clean clean-config
	@echo "Full clean completed"

.PHONY: all clean clean-build clean-config distclean show-config
