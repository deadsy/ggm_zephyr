/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#include "ggm.h"

#define GGM_VERSION "0.1"

void foo(void);

void main(void)
{
	char tmp[128];

	printk("GooGooMuck %s (%s)\n", GGM_VERSION, CONFIG_BOARD);
	printk("sin(30) = %s\n", ftoa(sinf(d2r(30.f)), tmp));
	printk("pow2(0.5) = %s\n", ftoa(pow2(0.5f), tmp));
	printk("powe(3.0) = %s\n", ftoa(powe(3.f), tmp));
}
