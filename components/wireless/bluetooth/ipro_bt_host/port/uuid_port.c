/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Port-layer implementation of the generic UUID string helper.
 *
 * upstream/zephyr/include/zephyr/sys/uuid.h declares uuid_to_string(), but the
 * matching upstream source (zephyr/lib/utils/uuid.c) is not part of this
 * import - only the header came across. The host's own
 * subsys/bluetooth/host/uuid.c calls it from bt_uuid_to_str() on the 128-bit
 * branch, so any build that formats a 128-bit UUID fails to link with
 * "undefined reference to uuid_to_string". LE Audio configurations reach that
 * branch, which is how it surfaces.
 *
 * The upstream tree is read-only by project rule, so the implementation lives
 * here in the port layer rather than being patched into upstream/. It follows
 * the RFC 9562 canonical form (8-4-4-4-12, lower case) that the header
 * documents.
 */

#include <zephyr/sys/uuid.h>

#include <errno.h>
#include <stdio.h>

int uuid_to_string(const struct uuid *data, char out[UUID_STR_LEN])
{
	if (data == NULL || out == NULL) {
		return -EINVAL;
	}

	(void)snprintf(out, UUID_STR_LEN,
		       "%02x%02x%02x%02x-%02x%02x-%02x%02x-%02x%02x-"
		       "%02x%02x%02x%02x%02x%02x",
		       data->val[0], data->val[1], data->val[2], data->val[3],
		       data->val[4], data->val[5], data->val[6], data->val[7],
		       data->val[8], data->val[9], data->val[10], data->val[11],
		       data->val[12], data->val[13], data->val[14], data->val[15]);

	return 0;
}
