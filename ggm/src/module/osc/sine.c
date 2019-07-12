/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * private state
 */

struct sine_osc {
	float freq;             /* base frequency */
	uint32_t x;             /* current x-value */
	uint32_t xstep;         /* current x-step */
};

/******************************************************************************
 * module port functions
 */

static void sine_port_frequency(struct module *m, struct event *e)
{
	float frequency = clampf_lo(event_get_float(e), 0);

	// log.Info.Printf("set frequency %f Hz", frequency)

	struct sine_osc *sine = (struct sine_osc *)m->priv;

	sine->freq = frequency;
	sine->xstep = (uint32_t)(frequency * FrequencyScale);
}

/******************************************************************************
 * module functions
 */

static int sine_init(struct module *m)
{
	return 0;
}

static int sine_stop(struct module *m)
{
	return 0;
}

static size_t sine_child(struct module *m, struct module **list)
{
	return 0;
}

static bool sine_process(struct module *m, float *buf[])
{
	return false;
}

/******************************************************************************
 * module information
 */

static struct port_info in_ports[] = {
	{ .name = "frequency", .type = PORT_TYPE_FLOAT, .func = sine_port_frequency },
};

static struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
};

static struct module_info sine_module = {
	.name = "sine",
	.in = in_ports,
	.out = out_ports,
	.init = sine_init,
	.stop = sine_stop,
	.child = sine_child,
	.process = sine_process,
};

MODULE_REGISTER(sine_module);
