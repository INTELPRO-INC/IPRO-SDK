IPRO_SDK_BASE ?= $(abspath ../../../)
export IPRO_SDK_BASE

all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)

clean:
	@rm -rf build

.PHONY: all clean
