/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

static void sinePortFrequency(struct module *m, struct event *e) {
}

static struct port_info inPorts[] = {
  {.name = "frequency", .type = PORT_TYPE_FLOAT, .func = sinePortFrequency},
};

static struct port_info outPorts[] = {
  {.name = "out", .type = PORT_TYPE_AUDIO,},
};

static struct module_info sineOscInfo  = {
  .name = "sineOsc",
  .in = inPorts,
  .out = outPorts,
};
