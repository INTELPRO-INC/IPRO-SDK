SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_unit_test

#
# Default target
#
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

#
# Configuration Management
#
KCONFIG_ROOT = apps/test/ipro_unit_test/Kconfig
CONFIG_FILE = $(PROJECT_PATH)/.config

# Interactive configuration (requires kconfiglib)
menuconfig:
	@if [ ! -f "$(CONFIG_FILE)" ]; then \
		echo "Error: .config not found"; \
		exit 1; \
	fi
	@KCONFIG_CONFIG=$(CONFIG_FILE) \
		srctree=$(IPRO_SDK_BASE) \
		menuconfig $(IPRO_SDK_BASE)/Kconfig

#
# Build targets
#
clean:
	@echo "Cleaning build directory"
	@rm -rf build
	@echo "Clean complete"

distclean: clean
	@echo "Removing configuration files"
	@rm -f $(CONFIG_FILE) $(CONFIG_FILE).old
	@echo "Distclean complete"

rebuild: clean all

#
# Board flash and test targets
#
SERIAL_PORT ?= /dev/cu.usbserial-2120
# IntelProCube v0.6.3 command-line flasher (ipro_iot_tool_macos / _linux / .exe).
# It has no --flash-pin: flash_pin lives in
# <IntelProCube dir>/chips/ipro7/eflash_loader/eflash_loader_cfg.ini.
FLASH_TOOL ?= $(IPRO_FLASH_TOOL)
# Internal tool; not part of customer SDK packages.
BOARD_TEST ?= $(IPRO_SDK_BASE)/tools/board_test.sh

# The tool exits 0 even when flashing fails; "[All Success]" is the verdict.
flash: all
	@test -n "$(FLASH_TOOL)" || { echo "Set IPRO_FLASH_TOOL (or FLASH_TOOL=) to the IntelProCube ipro_iot_tool binary"; exit 1; }
	$(FLASH_TOOL) \
		--chipname ipro7 \
		--port $(SERIAL_PORT) \
		--firmware $$(ls build/build_out/*.bin | head -1) \
		--baudrate 921600 2>&1 | tee build/flash.log
	@grep -q "\[All Success\]" build/flash.log || { echo "flash FAILED (no [All Success]); see build/flash.log"; exit 1; }

monitor:
	@stty -f $(SERIAL_PORT) 115200 raw -echo 2>/dev/null || stty -F $(SERIAL_PORT) 115200 raw -echo 2>/dev/null
	@echo "Monitoring $(SERIAL_PORT) at 115200 baud (Ctrl-C to exit)..."
	@cat $(SERIAL_PORT)

test:
	@test -x "$(BOARD_TEST)" || { echo "$(BOARD_TEST) is an internal tool and is not in SDK packages"; exit 1; }
	$(BOARD_TEST) --project apps/$(PROJECT_NAME) --port $(SERIAL_PORT)

#
# Analysis and debugging
#
size:
	@if [ -f "build/build_out/$(PROJECT_NAME).elf" ]; then \
		riscv64-unknown-elf-size build/build_out/$(PROJECT_NAME).elf; \
	else \
		echo "Error: ELF file not found. Build the project first."; \
	fi

dump:
	@if [ -f "build/build_out/$(PROJECT_NAME).elf" ]; then \
		riscv64-unknown-elf-objdump -d build/build_out/$(PROJECT_NAME).elf > build/build_out/$(PROJECT_NAME).asm; \
		echo "Disassembly saved to build/build_out/$(PROJECT_NAME).asm"; \
	else \
		echo "Error: ELF file not found. Build the project first."; \
	fi

#
# Help
#
help:
	@echo "IPRO SDK Unit Test Project"
	@echo ""
	@echo "Available targets:"
	@echo "  all (default)    - Build the unit test project"
	@echo "  clean            - Remove build directory"
	@echo "  distclean        - Remove build and configuration"
	@echo "  rebuild          - Clean and rebuild"
	@echo "  menuconfig       - Interactive configuration"
	@echo "  size             - Show binary size information"
	@echo "  dump             - Generate disassembly"
	@echo "  flash            - Flash firmware to board"
	@echo "  monitor          - Open serial monitor"
	@echo "  test             - Build, flash, and verify tests"
	@echo "  help             - Show this help message"
	@echo ""
	@echo "Usage:"
	@echo "  make             - Build project"
	@echo "  make clean       - Clean build files"
	@echo "  make menuconfig  - Configure project"

.PHONY: all clean distclean rebuild menuconfig size dump help flash monitor test
