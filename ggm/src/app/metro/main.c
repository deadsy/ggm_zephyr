/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#define GGM_MAIN

#include "ggm.h"
#include "module.h"

/******************************************************************************
 * main
 */

int main(void)
{
	LOG_INF("GooGooMuck %s (%s)", GGM_VERSION, CONFIG_BOARD);

	struct synth *s = synth_new();
	struct module *m = module_new(s, "seq.metro");
	synth_set_root(s, m);

  for(int i = 0; i < 3000; i ++) {
    synth_loop(s);
    ggm_mdelay(3);
  }

	synth_del(s);

  return 0;
}

/*****************************************************************************/
