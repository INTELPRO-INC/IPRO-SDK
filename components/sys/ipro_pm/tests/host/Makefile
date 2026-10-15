# Host unit tests for ipro_pm (no target toolchain needed).
CC ?= cc
CFLAGS += -std=c11 -Wall -Wextra -Werror -Istubs -I../../include
SRC = ../../src

TESTS = test_pm_core test_pm_arbiter test_pm_lpclk

all: run

test_pm_core: test_pm_core.c $(SRC)/pm_core.c
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $^

test_pm_arbiter: test_pm_arbiter.c $(SRC)/pm_arbiter.c
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $^

test_pm_lpclk: test_pm_lpclk.c $(SRC)/pm_lpclk.c
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $^

# golden equivalence: the port before the participant/arbiter refactor and
# the current one must make the same calls with only the BLE participant
equiv_golden: test_pm_freertos_equiv.c golden/pm_freertos_v1.c
	$(CC) $(CFLAGS) -DGOLDEN -Igolden -o $@ $^

equiv_current: test_pm_freertos_equiv.c $(SRC)/pm_freertos.c $(SRC)/pm_arbiter.c
	$(CC) $(CFLAGS) -I$(SRC) -o $@ $^

equiv: equiv_golden equiv_current
	./equiv_golden > equiv_golden.log
	./equiv_current > equiv_current.log
	diff -u equiv_golden.log equiv_current.log
	@echo "ipro_pm port golden equivalence: identical ($$(wc -l < equiv_current.log) lines)"

run: $(TESTS) equiv
	@for t in $(TESTS); do ./$$t || exit 1; done

clean:
	rm -f $(TESTS) equiv_golden equiv_current equiv_golden.log equiv_current.log

.PHONY: all run equiv clean
