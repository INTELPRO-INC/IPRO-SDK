# Host test of the NSPE driver PAL (port/nspe/pal_driver_intf.c):
# RAM NVMEM bounds and reset, system reset.
# make -C components/3rdparty/psa_arch_tests/port/tests test

CC := /usr/bin/clang
PORT := ../nspe
PAL := ../../psa_arch_tests/api-tests/platform/targets/common/nspe
CFLAGS := -Wall -Wextra -Werror -std=c99 \
          -fsanitize=address,undefined -fno-omit-frame-pointer \
          -I$(PORT) -I$(PAL) -I$(PAL)/crypto
BUILD := build

test: $(BUILD)/test_pal_nvmem
	./$(BUILD)/test_pal_nvmem

$(BUILD)/test_pal_nvmem: test_pal_nvmem.c $(PORT)/pal_driver_intf.c \
                         $(PORT)/pal_config.h | $(BUILD)
	$(CC) $(CFLAGS) -o $@ $(filter %.c,$^)

$(BUILD):
	mkdir -p $@

clean:
	rm -rf $(BUILD)

.PHONY: test clean
