/*
 * Copyright (c) 2026 INTELPRO
 * SPDX-License-Identifier: Apache-2.0
 *
 * IPRO7AI EVK demo on Zephyr: LCD, PDM microphone, speaker (GPDAC) and the
 * SC101IOT camera, driven from the shell ("evk ..."). See README.md.
 */
#include "demo.h"
#include <zephyr/sys/printk.h>

int main(void)
{
	int r;

	r = lcd_init();
	printk("evk demo: lcd %d (%ux%u)\n", r, lcd_width(), lcd_height());
	if (r == 0) {
		(void)lcd_bars();
	}
	r = audio_init();
	printk("evk demo: audio %d (mic pdm0 GPIO18/19, speaker GPDAC-A GPIO23, enable GPIO22)\n", r);
#ifdef CONFIG_EVK_DEMO_CAMERA
	r = cam_init();
	printk("evk demo: camera %d\n", r);
#endif
	printk("evk demo ready - type 'evk' for the commands\n");
	return 0;
}
