SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CPU_ID := m0
CHIP := ipro6
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro6_btdm_phyrf

#
# Default target
#
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

#
# Configuration Management (Linux-style)
#
DEFCONFIG ?= ipro6_btdm_phyrf_defconfig
KCONFIG_ROOT = apps/platform/ipro6_btdm_phyrf/Kconfig.app
CONFIG_FILE = $(PROJECT_PATH)/.config
DEFCONFIG_FILE = $(PROJECT_PATH)/defconfig

# Default configuration
defconfig:
	@echo "Loading defconfig: $(DEFCONFIG)"
	@if [ -f "$(IPRO_SDK_BASE)/configs/$(DEFCONFIG)" ]; then \
		cp $(IPRO_SDK_BASE)/configs/$(DEFCONFIG) $(CONFIG_FILE); \
		echo "Configuration loaded from configs/$(DEFCONFIG)"; \
	elif [ -f "$(DEFCONFIG_FILE)" ]; then \
		cp $(DEFCONFIG_FILE) $(CONFIG_FILE); \
		echo "Configuration loaded from local defconfig"; \
	else \
		echo "Error: defconfig file not found"; \
		echo "  Tried: configs/$(DEFCONFIG)"; \
		echo "  Tried: $(DEFCONFIG_FILE)"; \
		exit 1; \
	fi

# Interactive configuration (requires kconfiglib)
menuconfig:
	@if [ ! -f "$(CONFIG_FILE)" ]; then \
		if [ -f "$(IPRO_SDK_BASE)/configs/$(DEFCONFIG)" ]; then \
			echo "Creating .config from $(DEFCONFIG)"; \
			cp $(IPRO_SDK_BASE)/configs/$(DEFCONFIG) $(CONFIG_FILE); \
		elif [ -f "$(DEFCONFIG_FILE)" ]; then \
			echo "Creating .config from local defconfig"; \
			cp $(DEFCONFIG_FILE) $(CONFIG_FILE); \
		else \
			echo "Error: No configuration file found"; \
			echo "Run 'make defconfig' first to create initial configuration"; \
			exit 1; \
		fi \
	fi
	@cp $(CONFIG_FILE) $(CONFIG_FILE).old
	@cd $(PROJECT_PATH) && srctree=$(IPRO_SDK_BASE) KCONFIG_CONFIG=$(CONFIG_FILE) menuconfig $(KCONFIG_ROOT)
	@echo ""
	@echo "Configuration updated. Run 'make' to rebuild."

# Save current config as defconfig
savedefconfig:
	@if [ -f "$(CONFIG_FILE)" ]; then \
		cp $(CONFIG_FILE) $(DEFCONFIG_FILE); \
		echo "Configuration saved to defconfig"; \
	else \
		echo "Error: .config not found"; \
		exit 1; \
	fi

# List available defconfigs
list-defconfigs:
	@echo "Available defconfigs:"
	@ls -1 $(IPRO_SDK_BASE)/configs/*_defconfig 2>/dev/null | xargs -n1 basename || echo "  (none found)"

# Show current configuration
show-config:
	@if [ -f "$(CONFIG_FILE)" ]; then \
		echo "Current configuration (.config):"; \
		cat $(CONFIG_FILE); \
	else \
		echo "No configuration file found"; \
		echo "Run 'make defconfig' to create initial configuration"; \
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

.PHONY: all clean clean-build clean-config distclean \
        defconfig menuconfig savedefconfig list-defconfigs show-config
