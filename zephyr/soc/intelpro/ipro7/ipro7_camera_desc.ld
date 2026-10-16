/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * SDK sensor descriptor table (components/sensor): each sensor driver puts
 * its rt_camera_desc in .camera_desc and camera_module_init() walks the
 * table between the two symbols (same layout as bsp/board/linker).
 */
SECTION_PROLOGUE(.camera_desc,,)
{
	. = ALIGN(4);
	_cam_desc_start = .;
	cam_desc_start = .;
	KEEP(*(.camera_desc))
	_cam_desc_end = .;
	cam_desc_end = .;
} GROUP_LINK_IN(ROMABLE_REGION)
