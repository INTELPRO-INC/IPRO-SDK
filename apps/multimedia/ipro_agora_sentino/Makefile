SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= reef
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_agora_sentino

#
# Default target
#
#
# Board selection (same mechanism as ipro_pdm_mic_demo):
#   make              -> .config    (IPRO7AI Reef: IPRO6S_FIN Wi-Fi in the card
#   make BOARD=reef                  slot, on-chip BLE for provisioning, MSM261
#                                    PDM mic, DAC + PAM8302A, no LCD)
#   make BOARD=evk    -> config_evk (IPRO7AI EVK: AIC8800 Wi-Fi on the SD slot,
#                        on-board PDM mic, DAC + TPA2037D1, LCD off for now)
# Other board configs build in build_config_<board>/ so .config is left alone.
#
ifneq ($(BOARD),reef)
ifeq ($(wildcard $(PROJECT_PATH)/config_$(BOARD)),)
$(error no config_$(BOARD) in $(PROJECT_PATH); known boards: $(patsubst config_%,%,$(notdir $(wildcard $(PROJECT_PATH)/config_*))))
endif
BOARD_ARGS := DEFCONFIG=config_$(BOARD)
endif

all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) $(BOARD_ARGS)

#
# Configuration Management
#
KCONFIG_ROOT = apps/multimedia/ipro_agora_sentino/Kconfig
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
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) $(BOARD_ARGS) clean

clean-config:
	@rm -f $(CONFIG_FILE) $(CONFIG_FILE).old
	@echo "Configuration files cleaned"

distclean: clean clean-config
	@echo "Full clean completed"

.PHONY: all clean clean-build clean-config distclean menuconfig show-config
