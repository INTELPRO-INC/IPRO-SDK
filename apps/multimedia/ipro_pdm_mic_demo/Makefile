SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_pdm_mic_demo

#
# Board selection.
#
#   make              -> .config (the EVB layout; config_evb is its twin)
#   make BOARD=evk    -> config_evk, built in build_config_evk/ via the SDK's
#                        DEFCONFIG mechanism, so .config is left alone.
#
# BOARD used to be declared here and read by nothing, which is how an "EVK
# build" came out with the EVB pin map. If config_$(BOARD) does not exist the
# build stops rather than quietly using .config.
#
ifneq ($(BOARD),evb)
ifeq ($(wildcard $(PROJECT_PATH)/config_$(BOARD)),)
$(error no config_$(BOARD) in $(PROJECT_PATH); known boards: $(patsubst config_%,%,$(notdir $(wildcard $(PROJECT_PATH)/config_*))))
endif
BOARD_ARGS := DEFCONFIG=config_$(BOARD)
endif

#
# Default target
#
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) $(BOARD_ARGS)

#
# Build Targets
#
clean: clean-build

clean-build:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) $(BOARD_ARGS) clean

.PHONY: all clean clean-build
