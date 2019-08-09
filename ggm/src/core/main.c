/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr.h>
#include <sys/printk.h>

#define GGM_MAIN

#include "ggm.h"
#include "module.h"

#define GGM_VERSION "0.1"

/******************************************************************************
 * oscillator functions - returns a module which is an oscillator.
 */

#define OSC_NEW0

#ifdef OSC_NEW0
static struct module *osc_new(struct synth *s)
{
	return module_new(s, "sine");
}
#elif OSC_NEW1
static struct module *osc_new(struct synth *s)
{
	return module_new(s, "square_basic");
}
#elif OSC_NEW2
static struct module *osc_new(struct synth *s)
{
	return module_new(s, "pink2");
}
#elif OSC_NEW3
static struct module *osc_new(struct synth *s)
{
	return module_new(s, "sawtooth_basic");
}
#else
#error "you need to define an oscillator"
#endif

/******************************************************************************
 * main
 */

void main(void)
{
	LOG_INF("GooGooMuck %s (%s)", GGM_VERSION, CONFIG_BOARD);

	struct synth *s = synth_new();
	struct module *m0 = module_new(s, "osc_voice", osc_new);
	synth_set_root(s, m0);
	synth_del(s);
}

/*****************************************************************************/
