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

void main(void)
{
	char tmp[64];
	struct synth s;

	LOG_INF("GooGooMuck %s (%s)", GGM_VERSION, CONFIG_BOARD);

	struct module *m0 = module_new(&s, "sine");
	LOG_INF("%s", log_strdup(module_str(m0, tmp)));

	struct module *m1 = module_new(&s, "adsr");
	LOG_INF("%s", log_strdup(module_str(m1, tmp)));

	module_free(m0);
	module_free(m1);
}

/*****************************************************************************/
