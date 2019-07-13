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

static void sine_port_frequency(struct module *m, struct event e)
{
	float frequency = clampf_lo(event_get_float(e), 0);
	char tmp[64];

	LOG_INF("set frequency %s Hz", log_strdup(ftoa(frequency, tmp)));
	struct sine_osc *osc = (struct sine_osc *)m->priv;
	osc->freq = frequency;
	osc->xstep = (uint32_t)(frequency * FrequencyScale);
}

/******************************************************************************
 * module functions
 */

static int sine_init(struct module *m)
{
	/* allocate the private data */
	struct sine_osc *osc = k_calloc(1, sizeof(struct sine_osc));

	if (osc == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}

	m->priv = (void *)osc;
	sine_port_frequency(m, event_set_float(440.f));
	return 0;
}

static void sine_stop(struct module *m)
{
	k_free(m->priv);
}

static struct module **sine_child(struct module *m)
{
	/* no children */
	return NULL;
}

static bool sine_process(struct module *m, float *buf[])
{
	struct sine_osc *osc = (struct sine_osc *)m->priv;
	float *out = buf[0];

	for (int i = 0; i < AudioBufferSize; i++) {
		out[i] = cos_lookup(osc->x);
		osc->x += osc->xstep;
	}
	return true;
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

struct module_info sine_module = {
	.name = "sine",
	.in = in_ports,
	.out = out_ports,
	.init = sine_init,
	.stop = sine_stop,
	.child = sine_child,
	.process = sine_process,
};

MODULE_REGISTER(sine_module);
