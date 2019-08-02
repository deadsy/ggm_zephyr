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

/******************************************************************************
 * defines
 */

#define GGM_VERSION "0.1"

static struct module *osc_new(struct synth *s)
{
	return module_new(s, "sine");
}

void main(void)
{
	struct synth s;

	LOG_INF("GooGooMuck %s (%s)", GGM_VERSION, CONFIG_BOARD);

	struct module *m0 = module_new(&s, "osc_voice", osc_new);
	module_del(m0);

}

/*****************************************************************************/
