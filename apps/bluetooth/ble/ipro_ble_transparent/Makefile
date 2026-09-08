SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= fpga
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_ble_transparent

all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

DEFCONFIG ?= ipro_ble_transparent_defconfig
CONFIG_FILE = $(PROJECT_PATH)/.config
DEFCONFIG_FILE = $(PROJECT_PATH)/defconfig

defconfig:
	@echo "Loading defconfig: $(DEFCONFIG)"
	@if [ -f "$(IPRO_SDK_BASE)/configs/$(DEFCONFIG)" ]; then \
		cp $(IPRO_SDK_BASE)/configs/$(DEFCONFIG) $(CONFIG_FILE); \
	elif [ -f "$(DEFCONFIG_FILE)" ]; then \
		cp $(DEFCONFIG_FILE) $(CONFIG_FILE); \
	else \
		echo "Error: defconfig file not found"; \
		exit 1; \
	fi

savedefconfig:
	@if [ -f "$(CONFIG_FILE)" ]; then \
		cp $(CONFIG_FILE) $(DEFCONFIG_FILE); \
	else \
		echo "Error: .config not found"; \
		exit 1; \
	fi

clean: clean-build

clean-build:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) clean

distclean: clean
	@rm -f $(CONFIG_FILE) $(CONFIG_FILE).old

.PHONY: all clean clean-build distclean defconfig savedefconfig
