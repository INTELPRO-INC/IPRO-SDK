# Host unit tests for the ipronet OTA protocol against the device core;
# not part of the firmware build.
#   make        build and run
#   make clean  remove the binary
CC ?= cc
RNM = ../../remote_net_manager
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -I.. -I$(RNM)/src -I$(RNM)/tests
SRCS = test_ipronet_ota_proto.c ../ipronet_ota_proto.c $(RNM)/src/rnm_ota_core.c

all: test_ipronet_ota_proto

test_ipronet_ota_proto: $(SRCS) ../ipronet_ota_proto.h ../ipronet_ota.h ../ipronet_ctrl.h $(RNM)/tests/ota_fake_store.h
	$(CC) $(CFLAGS) -o $@ $(SRCS)
	./$@

.PHONY: all clean
clean:
	rm -f test_ipronet_ota_proto
	rm -rf test_ipronet_ota_proto.dSYM
