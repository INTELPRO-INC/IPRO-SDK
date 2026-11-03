IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP := ipro6
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro6_fmac
DEFCONFIG ?= defconfig_ipro6_fmac

all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) DEFCONFIG=$(DEFCONFIG)

KCONFIG_ROOT = apps/platform/ipro6_fmac/Kconfig.app

clean:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) DEFCONFIG=$(DEFCONFIG) clean

.PHONY: all clean
