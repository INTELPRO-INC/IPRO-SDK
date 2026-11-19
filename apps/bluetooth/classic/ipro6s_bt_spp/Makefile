SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../../)

export IPRO_SDK_BASE

CPU_ID := m0
CHIP := ipro6
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro6s_bt_spp

# Build the app (DEFCONFIG=defconfig_<name> builds that configuration
# into build_<name>/ instead of .config into build/).
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

KCONFIG_ROOT = apps/bluetooth/classic/ipro6s_bt_spp/Kconfig.app
CONFIG_FILE = $(PROJECT_PATH)/.config

# Interactive configuration (requires kconfiglib)
menuconfig:
	@cp $(CONFIG_FILE) $(CONFIG_FILE).old
	@cd $(PROJECT_PATH) && srctree=$(IPRO_SDK_BASE) KCONFIG_CONFIG=$(CONFIG_FILE) menuconfig $(KCONFIG_ROOT)

clean:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) clean

.PHONY: all clean menuconfig
