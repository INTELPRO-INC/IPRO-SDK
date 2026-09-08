/* SPDX-License-Identifier: Apache-2.0 */

#include <errno.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include <zephyr/settings/settings.h>
#include <zephyr/sys/iterable_sections.h>
#include <zephyr/sys/slist.h>
#include <zephyr/sys/util.h>

#if defined(CONFIG_EASYFLASH4)
#include <easyflash.h>
#endif

#include "ipro_log.h"

#define IPRO_LOG_TAG "BT_SETTINGS"

static sys_slist_t runtime_handlers = SYS_SLIST_STATIC_INIT(&runtime_handlers);

struct runtime_read_arg {
	const void *data;
	size_t len;
};

static ssize_t runtime_read_cb(void *cb_arg, void *buf, size_t buf_len)
{
	struct runtime_read_arg *arg = cb_arg;
	size_t read_len;

	if ((arg == NULL) || (buf == NULL)) {
		return -EINVAL;
	}

	read_len = MIN(buf_len, arg->len);
	if (read_len != 0U) {
		memcpy(buf, arg->data, read_len);
	}

	return (ssize_t)read_len;
}

#if defined(CONFIG_EASYFLASH4)
static const struct settings_load_arg *active_load_arg;
static unsigned int active_load_rejected;

#if defined(CONFIG_IPRO_BT_HOST_SMP_TRACE)
static const char *easyflash_err_name(EfErrCode err)
{
	switch (err) {
	case EF_NO_ERR:
		return "EF_NO_ERR";
	case EF_ERASE_ERR:
		return "EF_ERASE_ERR";
	case EF_READ_ERR:
		return "EF_READ_ERR";
	case EF_WRITE_ERR:
		return "EF_WRITE_ERR";
	case EF_ENV_NAME_ERR:
		return "EF_ENV_NAME_ERR";
	case EF_ENV_NAME_EXIST:
		return "EF_ENV_NAME_EXIST";
	case EF_ENV_FULL:
		return "EF_ENV_FULL";
	case EF_ENV_INIT_FAILED:
		return "EF_ENV_INIT_FAILED";
	case EF_ENV_ARG_ERR:
		return "EF_ENV_ARG_ERR";
	default:
		return "EF_ERR_UNKNOWN";
	}
}
#endif
#endif

static bool settings_name_matches(const char *name, const char *tree,
				  const char **next)
{
	size_t tree_len;

	if ((name == NULL) || (tree == NULL)) {
		if (next != NULL) {
			*next = NULL;
		}
		return false;
	}

	tree_len = strlen(tree);
	if ((tree_len == 0U) || (strncmp(name, tree, tree_len) != 0)) {
		if (next != NULL) {
			*next = NULL;
		}
		return false;
	}

	if ((name[tree_len] != '\0') &&
	    (name[tree_len] != SETTINGS_NAME_SEPARATOR)) {
		if (next != NULL) {
			*next = NULL;
		}
		return false;
	}

	if (next != NULL) {
		*next = (name[tree_len] == SETTINGS_NAME_SEPARATOR) ?
			&name[tree_len + 1U] : NULL;
	}

	return true;
}

int settings_name_next(const char *name, const char **next)
{
	const char *sep;

	if (name == NULL) {
		if (next != NULL) {
			*next = NULL;
		}
		return 0;
	}

	sep = strchr(name, SETTINGS_NAME_SEPARATOR);
	if (sep == NULL) {
		if (next != NULL) {
			*next = NULL;
		}
		return (int)strlen(name);
	}

	if (next != NULL) {
		*next = sep + 1;
	}

	return (int)(sep - name);
}

int settings_name_steq(const char *name, const char *key, const char **next)
{
	size_t key_len;

	if ((name == NULL) || (key == NULL)) {
		if (next != NULL) {
			*next = NULL;
		}
		return 0;
	}

	key_len = strlen(key);
	if ((key_len == 0U) || (key[key_len - 1U] == SETTINGS_NAME_SEPARATOR) ||
	    (strncmp(name, key, key_len) != 0)) {
		if (next != NULL) {
			*next = NULL;
		}
		return 0;
	}

	if (next != NULL) {
		*next = (name[key_len] == SETTINGS_NAME_SEPARATOR) ?
			&name[key_len + 1U] : &name[key_len];
	}

	return 1;
}

static struct settings_handler_static *
lookup_runtime_handler(const char *name, const char **next,
		       size_t *best_len)
{
	sys_snode_t *node;
	struct settings_handler_static *best = NULL;

	SYS_SLIST_FOR_EACH_NODE(&runtime_handlers, node) {
		struct settings_handler *handler =
			SYS_SLIST_CONTAINER(node, handler, node);
		const char *candidate_next;
		size_t len;

		if (!settings_name_matches(name, handler->name, &candidate_next)) {
			continue;
		}

		len = strlen(handler->name);
		if (len <= *best_len) {
			continue;
		}

		best = (struct settings_handler_static *)handler;
		*best_len = len;
		if (next != NULL) {
			*next = candidate_next;
		}
	}

	return best;
}

struct settings_handler_static *settings_parse_and_lookup(const char *name,
							  const char **next)
{
	struct settings_handler_static *best = NULL;
	size_t best_len = 0U;

	STRUCT_SECTION_FOREACH(settings_handler_static, handler) {
		const char *candidate_next;
		size_t len;

		if (!settings_name_matches(name, handler->name, &candidate_next)) {
			continue;
		}

		len = strlen(handler->name);
		if (len <= best_len) {
			continue;
		}

		best = handler;
		best_len = len;
		if (next != NULL) {
			*next = candidate_next;
		}
	}

	{
		struct settings_handler_static *runtime =
			lookup_runtime_handler(name, next, &best_len);

		if (runtime != NULL) {
			best = runtime;
		}
	}

	if ((best == NULL) && (next != NULL)) {
		*next = NULL;
	}

	return best;
}

int settings_call_set_handler(const char *name, size_t len,
			      settings_read_cb read_cb, void *read_cb_arg,
			      const struct settings_load_arg *load_arg)
{
	const char *next = NULL;
	struct settings_handler_static *handler;

	if ((load_arg != NULL) && (load_arg->cb != NULL)) {
		return load_arg->cb(name, len, read_cb, read_cb_arg,
				    load_arg->param);
	}

	handler = settings_parse_and_lookup(name, &next);
	if ((handler == NULL) || (handler->h_set == NULL)) {
		return 0;
	}

	return handler->h_set(next, len, read_cb, read_cb_arg);
}

int settings_register_with_cprio(struct settings_handler *cf, int cprio)
{
	if ((cf == NULL) || (cf->name == NULL)) {
		return -EINVAL;
	}

	cf->cprio = cprio;
	sys_slist_append(&runtime_handlers, &cf->node);
	return 0;
}

int settings_register(struct settings_handler *cf)
{
	return settings_register_with_cprio(cf, 0);
}

void settings_src_register(struct settings_store *cs)
{
	ARG_UNUSED(cs);
}

void settings_dst_register(struct settings_store *cs)
{
	ARG_UNUSED(cs);
}

static int settings_backend_ready(void)
{
#if defined(CONFIG_EASYFLASH4)
	return (easyflash_init() == EF_NO_ERR) ? 0 : -EIO;
#else
	return -ENOTSUP;
#endif
}

#if defined(CONFIG_EASYFLASH4)
static ssize_t easyflash_read_cb(void *cb_arg, void *data, size_t len)
{
	env_node_obj_t env = cb_arg;
	size_t read_len;

	if ((env == NULL) || (data == NULL)) {
		return -EINVAL;
	}

	read_len = MIN(len, (size_t)env->value_len);
	if (read_len == 0U) {
		return 0;
	}

	if (ef_port_read(env->addr.value, (uint32_t *)data, read_len) != EF_NO_ERR) {
		return -EIO;
	}

	return (ssize_t)read_len;
}

static bool easyflash_load_cb(env_node_obj_t env, void *arg1, void *arg2)
{
	char name[EF_ENV_NAME_MAX + 1U];

	ARG_UNUSED(arg1);
	ARG_UNUSED(arg2);

	if ((env == NULL) || !env->crc_is_ok || (env->status != ENV_WRITE)) {
		return false;
	}

	if (env->name_len >= sizeof(name)) {
		/* Not ours either - same treatment as a rejected entry. */
		active_load_rejected++;
		return false;
	}

	memcpy(name, env->name, env->name_len);
	name[env->name_len] = '\0';

#if defined(CONFIG_IPRO_BT_HOST_SMP_TRACE)
	IPRO_LOGD(IPRO_LOG_TAG, "load name=%s len=%u value_len=%u", name, (unsigned)env->name_len, (unsigned)env->value_len);
#endif

	if ((active_load_arg != NULL) && (active_load_arg->subtree != NULL) &&
	    !settings_name_matches(name, active_load_arg->subtree, NULL)) {
		return false;
	}

	/*
	 * A handler that refuses an entry must not stop the load.
	 *
	 * The PSM partition outlives the firmware. An entry written by a build
	 * with a different feature set - bt/irk from one that had privacy on,
	 * a key format this build does not know - reaches a handler that
	 * answers -ENOENT, and that is all it means: "not mine". Returning true
	 * here ended the iteration on the first such entry, so nothing after it
	 * loaded and, worse, settings_load_subtree() never reached the commit.
	 * With CONFIG_BT_SETTINGS the commit is where the host sets its identity
	 * and BT_DEV_READY; without it every bt_le_adv_start() answers -EAGAIN
	 * and the device is silent for as long as the stale entry exists. Seen
	 * on an EVK that had run the LE Audio headset before this firmware.
	 *
	 * So: say which entry, skip it, carry on. The bond it may have belonged
	 * to is simply not loaded and the phone pairs again, which is the
	 * self-healing outcome.
	 */
	int err = settings_call_set_handler(name, env->value_len,
					    easyflash_read_cb, env,
					    active_load_arg);
	if (err != 0) {
		IPRO_LOGW(IPRO_LOG_TAG, "%s rejected by its handler (%d) - skipped", name, err);
		active_load_rejected++;
	}
	return false;
}
#endif

static int settings_load_arg_execute(const struct settings_load_arg *arg)
{
	int err = settings_backend_ready();

	if (err != 0) {
		return err;
	}

#if defined(CONFIG_EASYFLASH4)
	active_load_arg = arg;
	active_load_rejected = 0;
	ef_print_env_cb(easyflash_load_cb);
	active_load_arg = NULL;
	if (active_load_rejected != 0) {
		IPRO_LOGW(IPRO_LOG_TAG,
			  "%u stored entr%s not understood by this build; the rest loaded",
			  active_load_rejected, active_load_rejected == 1 ? "y" : "ies");
	}
	/* Rejected entries are reported above, not returned: the caller's next
	 * step is the commit, and that must run. */
	return 0;
#else
	ARG_UNUSED(arg);
	return -ENOTSUP;
#endif
}

int settings_subsys_init(void)
{
	sys_slist_init(&runtime_handlers);
	return settings_backend_ready();
}

int settings_commit_subtree(const char *subtree)
{
	int err = 0;

	STRUCT_SECTION_FOREACH(settings_handler_static, handler) {
		if ((handler->h_commit == NULL) ||
		    ((subtree != NULL) &&
		     !settings_name_matches(handler->name, subtree, NULL))) {
			continue;
		}
		err = handler->h_commit();
		if (err != 0) {
			return err;
		}
	}

	{
		sys_snode_t *node;

		SYS_SLIST_FOR_EACH_NODE(&runtime_handlers, node) {
			struct settings_handler *handler =
				SYS_SLIST_CONTAINER(node, handler, node);

			if ((handler->h_commit == NULL) ||
			    ((subtree != NULL) &&
			     !settings_name_matches(handler->name, subtree, NULL))) {
				continue;
			}
			err = handler->h_commit();
			if (err != 0) {
				return err;
			}
		}
	}

	return 0;
}

int settings_commit(void)
{
	return settings_commit_subtree(NULL);
}

int settings_load_subtree(const char *subtree)
{
	const struct settings_load_arg arg = {
		.subtree = subtree,
		.cb = NULL,
		.param = NULL,
	};
	int err = settings_load_arg_execute(&arg);

	if (err != 0) {
		return err;
	}

	return settings_commit_subtree(subtree);
}

int settings_load(void)
{
	return settings_load_subtree(NULL);
}

int settings_load_subtree_direct(const char *subtree,
				 settings_load_direct_cb cb, void *param)
{
	const struct settings_load_arg arg = {
		.subtree = subtree,
		.cb = cb,
		.param = param,
	};

	return settings_load_arg_execute(&arg);
}

ssize_t settings_load_one(const char *name, void *buf, size_t buf_len)
{
#if defined(CONFIG_EASYFLASH4)
	size_t saved_len = 0U;
	size_t read_len;
	int err = settings_backend_ready();

	if (err != 0) {
		return err;
	}
	if ((name == NULL) || (buf == NULL)) {
		return -EINVAL;
	}
	if (strlen(name) > EF_ENV_NAME_MAX) {
		return -ENAMETOOLONG;
	}

	read_len = ef_get_env_blob(name, buf, buf_len, &saved_len);
	if ((read_len == 0U) && (saved_len == 0U)) {
		return -ENOENT;
	}
	if (saved_len > buf_len) {
		return -ENOMEM;
	}

	return (ssize_t)read_len;
#else
	ARG_UNUSED(name);
	ARG_UNUSED(buf);
	ARG_UNUSED(buf_len);
	return -ENOTSUP;
#endif
}

ssize_t settings_get_val_len(const char *key)
{
#if defined(CONFIG_EASYFLASH4)
	uint8_t tmp;
	size_t saved_len = 0U;
	int err = settings_backend_ready();

	if (err != 0) {
		return err;
	}
	if (key == NULL) {
		return -EINVAL;
	}
	if (strlen(key) > EF_ENV_NAME_MAX) {
		return -ENAMETOOLONG;
	}

	(void)ef_get_env_blob(key, &tmp, sizeof(tmp), &saved_len);
	return (ssize_t)saved_len;
#else
	ARG_UNUSED(key);
	return -ENOTSUP;
#endif
}

int settings_save_one(const char *name, const void *value, size_t val_len)
{
#if defined(CONFIG_EASYFLASH4)
	int err = settings_backend_ready();
	EfErrCode ef_err;

	if (err != 0) {
		return err;
	}
	if (name == NULL) {
		return -EINVAL;
	}
	if (strlen(name) > EF_ENV_NAME_MAX) {
		return -ENAMETOOLONG;
	}
	if (value == NULL) {
		return settings_delete(name);
	}

	ef_err = ef_set_env_blob(name, value, val_len);
#if defined(CONFIG_IPRO_BT_HOST_SMP_TRACE)
	IPRO_LOGD(IPRO_LOG_TAG, "save name=%s value_len=%u ef=%d(%s)", name, (unsigned)val_len, (int)ef_err, easyflash_err_name(ef_err));
#endif
	return (ef_err == EF_NO_ERR) ? 0 : -EIO;
#else
	ARG_UNUSED(name);
	ARG_UNUSED(value);
	ARG_UNUSED(val_len);
	return -ENOTSUP;
#endif
}

int settings_delete(const char *name)
{
#if defined(CONFIG_EASYFLASH4)
	int err = settings_backend_ready();
	EfErrCode ef_err;

	if (err != 0) {
		return err;
	}
	if (name == NULL) {
		return -EINVAL;
	}
	if (strlen(name) > EF_ENV_NAME_MAX) {
		return -ENAMETOOLONG;
	}

	ef_err = ef_del_env(name);
#if defined(CONFIG_IPRO_BT_HOST_SMP_TRACE)
	IPRO_LOGD(IPRO_LOG_TAG, "delete name=%s ef=%d(%s)", name, (int)ef_err, easyflash_err_name(ef_err));
#endif
	return ((ef_err == EF_NO_ERR) || (ef_err == EF_ENV_NAME_ERR)) ? 0 : -EIO;
#else
	ARG_UNUSED(name);
	return -ENOTSUP;
#endif
}

static int export_setting(const char *name, const void *val, size_t val_len)
{
	return settings_save_one(name, val, val_len);
}

int settings_save_subtree(const char *subtree)
{
	int err = 0;

	STRUCT_SECTION_FOREACH(settings_handler_static, handler) {
		if ((handler->h_export == NULL) ||
		    ((subtree != NULL) &&
		     !settings_name_matches(handler->name, subtree, NULL))) {
			continue;
		}
		err = handler->h_export(export_setting);
		if (err != 0) {
			return err;
		}
	}

	{
		sys_snode_t *node;

		SYS_SLIST_FOR_EACH_NODE(&runtime_handlers, node) {
			struct settings_handler *handler =
				SYS_SLIST_CONTAINER(node, handler, node);

			if ((handler->h_export == NULL) ||
			    ((subtree != NULL) &&
			     !settings_name_matches(handler->name, subtree, NULL))) {
				continue;
			}
			err = handler->h_export(export_setting);
			if (err != 0) {
				return err;
			}
		}
	}

	return 0;
}

int settings_save(void)
{
	return settings_save_subtree(NULL);
}

int settings_save_subtree_or_single_without_modification(
	const char *name, bool save_if_subtree, bool save_if_single_setting)
{
	ARG_UNUSED(save_if_single_setting);

	if (save_if_subtree) {
		return settings_save_subtree(name);
	}

	return 0;
}

int settings_runtime_set(const char *name, const void *data, size_t len)
{
	struct runtime_read_arg read_arg = {
		.data = data,
		.len = len,
	};
	struct settings_handler_static *handler;
	const char *next = NULL;

	handler = settings_parse_and_lookup(name, &next);
	if ((handler == NULL) || (handler->h_set == NULL)) {
		return -ENOENT;
	}

	return handler->h_set(next, len, runtime_read_cb, &read_arg);
}

int settings_runtime_get(const char *name, void *data, size_t len)
{
	struct settings_handler_static *handler;
	const char *next = NULL;

	handler = settings_parse_and_lookup(name, &next);
	if ((handler == NULL) || (handler->h_get == NULL)) {
		return -ENOENT;
	}

	return handler->h_get(next, data, len);
}

int settings_runtime_commit(const char *name)
{
	return settings_commit_subtree(name);
}
