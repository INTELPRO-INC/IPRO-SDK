/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * components/utils/ipro_log.h backend for Zephyr: the SDK's IPRO_LOG*()
 * macros end up in printk() (and therefore the Zephyr console / log
 * backend) instead of the SDK's own logger task.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <stdarg.h>
#include <stdbool.h>

#include "ipro_log.h"

static ipro_log_level_t global_level = IPRO_LOG_LEVEL_INFO;

static const char level_char[IPRO_LOG_LEVEL_MAX] = {'N', 'E', 'W', 'I', 'D', 'V'};

bool ipro_log_init(const ipro_log_config_t *config)
{
	if (config != NULL) {
		global_level = config->global_level;
	}
	return true;
}

bool ipro_log_level_enabled(ipro_log_level_t level)
{
	return level != IPRO_LOG_LEVEL_NONE && level <= global_level;
}

void ipro_log_set_level(ipro_log_level_t level)
{
	global_level = level;
}

ipro_log_level_t ipro_log_get_level(void)
{
	return global_level;
}

void ipro_log_writev(ipro_log_level_t level, const char *tag, const char *format, va_list args)
{
	if (!ipro_log_level_enabled(level) || level >= IPRO_LOG_LEVEL_MAX) {
		return;
	}
	printk("%c (%u) %s: ", level_char[level], (unsigned int)k_uptime_get_32(),
	       tag != NULL ? tag : "");
	vprintk(format, args);
	printk("\n");
}

void ipro_log_write(ipro_log_level_t level, const char *tag, const char *format, ...)
{
	va_list args;

	va_start(args, format);
	ipro_log_writev(level, tag, format, args);
	va_end(args);
}
