/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#include "lut.h"
#include "utils.h"

#define GGM_VERSION "0.1"

void main(void)
{
	printk("GooGooMuck %s (%s)\n", GGM_VERSION, CONFIG_BOARD);
	printk("sin 30 = 0x%08x\n", float2hex(sin_eval(d2r(30.f))));
}
