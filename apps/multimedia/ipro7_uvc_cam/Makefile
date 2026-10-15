SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro7_uvc_cam

#
# Default target
#
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

#
# Configuration Management
#
KCONFIG_ROOT = apps/multimedia/ipro7_uvc_cam/Kconfig.app
CONFIG_FILE = $(PROJECT_PATH)/.config

menuconfig:
	@if [ ! -f "$(CONFIG_FILE)" ]; then \
		echo "Error: .config not found"; \
		exit 1; \
	fi
	@cp $(CONFIG_FILE) $(CONFIG_FILE).old
	@cd $(PROJECT_PATH) && srctree=$(IPRO_SDK_BASE) KCONFIG_CONFIG=$(CONFIG_FILE) menuconfig $(KCONFIG_ROOT)
	@echo ""
	@echo "Configuration updated. Run 'make' to rebuild."

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

.PHONY: all clean clean-build clean-config distclean menuconfig show-config
