SDK_DEMO_PATH ?= .
IPRO_SDK_BASE ?= $(abspath ../../../)

export IPRO_SDK_BASE

CHIP ?= ipro7
BOARD ?= evb
PROJECT_PATH ?= $(abspath .)
PROJECT_NAME ?= ipro_npu_opverify

all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

clean:
	@echo "Cleaning build directory"
	@rm -rf build
	@echo "Clean complete"

distclean: clean
	@echo "Removing configuration files"
	@rm -f .config .config.old
	@echo "Distclean complete"

rebuild: clean all

.PHONY: all clean distclean rebuild
