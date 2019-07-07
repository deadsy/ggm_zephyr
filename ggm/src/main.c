/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#define GGM_VERSION "0.1"

void main(void)
{
	printk("GooGooMuck %s (%s)\n", GGM_VERSION, CONFIG_BOARD);
}
