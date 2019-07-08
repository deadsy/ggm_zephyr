/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#include "ggm.h"

#define GGM_VERSION "0.1"

void main(void)
{
	printk("GooGooMuck %s (%s)\n", GGM_VERSION, CONFIG_BOARD);
	printk("sin(30) = 0x%08x\n", float2uint(sinf(d2r(30.f))));
	printk("pow2(0.5) = 0x%08x\n", float2uint(pow2(0.5f)));
	printk("powe(3.0) = 0x%08x\n", float2uint(powe(3.f)));
}
