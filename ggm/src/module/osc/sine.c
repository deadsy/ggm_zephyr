/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * private state
 */

struct sineOsc {
	float freq;             /* base frequency */
	uint32_t x;             /* current x-value */
	uint32_t xstep;         /* current x-step */
};

/******************************************************************************
 * module port functions
 */

static void sinePortFrequency(struct module *m, struct event *e)
{
}

/******************************************************************************
 * module functions
 */

static void sineOscInit(struct module *m)
{
}

static void sineOscStop(struct module *m)
{
}

static void sineOscChild(struct module *m)
{
}

static void sineOscProcess(struct module *m)
{
}

/******************************************************************************
 * module information
 */

static struct port_info inPorts[] = {
	{ .name = "frequency", .type = PORT_TYPE_FLOAT, .func = sinePortFrequency },
};

static struct port_info outPorts[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
};

static struct module_info sineOscInfo = {
	.name = "sineOsc",
	.in = inPorts,
	.out = outPorts,
	.init = sineOscInit,
	.stop = sineOscStop,
	.child = sineOscChild,
	.process = sineOscProcess,
};
