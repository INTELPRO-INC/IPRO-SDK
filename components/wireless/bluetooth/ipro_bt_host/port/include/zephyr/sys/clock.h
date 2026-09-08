/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_SYS_CLOCK_H_
#define IPRO_BT_HOST_ZEPHYR_SYS_CLOCK_H_

#include <zephyr/kernel.h>

typedef int32_t k_ticks_t;

#define Z_TIMEOUT_NO_WAIT K_NO_WAIT
#define Z_TIMEOUT_TICKS(t) ((k_timeout_t)(t))
#define Z_FOREVER K_FOREVER

static inline k_timeout_t z_timeout_sum(k_timeout_t t1, k_timeout_t t2)
{
	if (t1 == K_FOREVER || t2 == K_FOREVER) {
		return K_FOREVER;
	}

	return t1 + t2;
}

static inline int sys_timepoint_cmp(k_timepoint_t a, k_timepoint_t b)
{
	if (a == b) {
		return 0;
	}

	return a < b ? -1 : 1;
}

static inline bool sys_timepoint_expired(k_timepoint_t timepoint)
{
	return sys_timepoint_timeout(timepoint) == K_NO_WAIT;
}

#endif /* IPRO_BT_HOST_ZEPHYR_SYS_CLOCK_H_ */
