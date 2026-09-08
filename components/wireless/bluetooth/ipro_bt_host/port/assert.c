/* SPDX-License-Identifier: Apache-2.0 */
#include <stdarg.h>
#include <stdio.h>

#include <zephyr/kernel.h>

void assert_print(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	(void)vprintf(fmt, args);
	va_end(args);
}

void assert_post_action(const char *file, unsigned int line)
{
	/* Deliberately printf, not IPRO_LOG*: ipro_log_write() takes a mutex, and
	 * by the time we are here the assertion has already fired - k_oops() is the
	 * next statement. A logger that can block or assert again is the wrong tool
	 * on the way down. */
	printf("\r\n[ipro_bt_host] fatal assertion at %s:%u\r\n", file, line);
	k_oops();
}

int snprintk(char *str, size_t size, const char *fmt, ...)
{
	int ret;
	va_list args;
	va_start(args, fmt);
	ret = vsnprintf(str, size, fmt, args);
    va_end(args);
    return ret;
}

int vsnprintk(char *str, size_t size, const char *fmt, va_list ap)
{
    return vsnprintf(str, size, fmt, ap);
}
