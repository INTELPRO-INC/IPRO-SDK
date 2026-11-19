IPRO_SDK_BASE ?= $(abspath ../../../..)
export IPRO_SDK_BASE
all:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR)
clean:
	$(MAKE) -C $(IPRO_SDK_BASE) -f project.build PROJECT_PATH=$(CURDIR) clean
.PHONY: all clean
