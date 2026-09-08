/* SPDX-License-Identifier: Apache-2.0 */
#ifndef IPRO_BT_HOST_ZEPHYR_SETTINGS_H_
#define IPRO_BT_HOST_ZEPHYR_SETTINGS_H_
#include <stdbool.h>
#include <stddef.h>
#include <sys/types.h>

#if defined(CONFIG_BT_SETTINGS) || defined(CONFIG_SETTINGS)

#include_next <zephyr/settings/settings.h>

#else

/* Keep the settings ABI visible even when persistence is disabled: upstream
 * code type-checks unreachable IS_ENABLED(CONFIG_BT_SETTINGS) branches. */
typedef ssize_t (*settings_read_cb)(void *cb_arg, void *data, size_t len);
typedef int (*settings_load_direct_cb)(const char *key, size_t len,
				       settings_read_cb read_cb, void *cb_arg,
				       void *param);

static inline int settings_load_subtree_direct(const char *subtree,
					settings_load_direct_cb cb,
					void *param)
{
	(void)subtree;
	(void)cb;
	(void)param;
	return 0;
}

static inline int settings_name_next(const char *name, const char **next)
{
	const char *slash = name;
	while (*slash != '\0' && *slash != '/') {
		slash++;
	}
	*next = *slash == '/' ? slash + 1 : NULL;
	return (int)(slash - name);
}

static inline bool settings_name_steq(const char *name, const char *key,
				      const char **next)
{
	while (*name != '\0' && *key != '\0' && *name == *key) {
		name++;
		key++;
	}
	if (*key != '\0' || (*name != '\0' && *name != '/')) {
		return false;
	}
	*next = *name == '/' ? name + 1 : NULL;
	return true;
}
#endif

#endif
