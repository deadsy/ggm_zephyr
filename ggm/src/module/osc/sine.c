/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

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

static void sine_port_frequency(struct module *m, const struct event *e)
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

static int sine_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct sine_osc *this = ggm_calloc(1, sizeof(struct sine_osc));

	if (this == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}

	m->priv = (void *)this;
	return 0;
}

static void sine_free(struct module *m)
{
	LOG_MOD_NAME(m);
	ggm_free(m->priv);
}

static bool sine_process(struct module *m, float *buf[])
{
	struct sine_osc *this = (struct sine_osc *)m->priv;
	float *out = buf[0];

	for (int i = 0; i < AudioBufferSize; i++) {
		out[i] = cos_lookup(this->x);
		this->x += this->xstep;
	}
	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "frequency", .type = PORT_TYPE_FLOAT, .func = sine_port_frequency },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info sine_module = {
	.name = "osc.sine",
	.in = in_ports,
	.out = out_ports,
	.alloc = sine_alloc,
	.free = sine_free,
	.process = sine_process,
};

MODULE_REGISTER(sine_module);

/*****************************************************************************/
