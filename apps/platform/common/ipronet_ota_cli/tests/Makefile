# Host unit tests for the OTA HTTP parser; not part of the firmware build.
#   make        build and run
#   make clean  remove the binary
CC ?= cc
CFLAGS = -Wall -Wextra -Werror -std=gnu11 -g -I..

all: test_ota_http

test_ota_http: test_ota_http.c ../ota_http_parse.c ../ota_http_fetch.h
	$(CC) $(CFLAGS) -o $@ test_ota_http.c ../ota_http_parse.c
	./$@

.PHONY: all clean
clean:
	rm -f test_ota_http
	rm -rf test_ota_http.dSYM
