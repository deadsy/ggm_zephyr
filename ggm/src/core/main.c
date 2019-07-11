/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#include "proto.h"
#include "util.h"

#define GGM_VERSION "0.1"


uint32_t foo(void)
{
	return STRHASH("this is my string");
}


void main(void)
{
	char tmp[128];
	uint32_t state;

	printk("GooGooMuck %s (%s)\n", GGM_VERSION, CONFIG_BOARD);
	printk("%08x\n", foo());

	rand_init(1234, &state);
	for (int i = 0; i < 100; i++) {
		float x = randf(&state);
		printk("%s %08x\n", ftoa(x, tmp), float2uint(x));
	}
}
