# Host unit tests for the RNM OTA core; not part of the firmware build.
#   make        build and run
#   make clean  remove the binary
CC ?= cc
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -I../src

all: test_rnm_ota_core

test_rnm_ota_core: test_rnm_ota_core.c ../src/rnm_ota_core.c ota_fake_store.h ../src/rnm_ota_core.h
	$(CC) $(CFLAGS) -o $@ test_rnm_ota_core.c ../src/rnm_ota_core.c
	./$@

.PHONY: all clean
clean:
	rm -f test_rnm_ota_core
	rm -rf test_rnm_ota_core.dSYM
