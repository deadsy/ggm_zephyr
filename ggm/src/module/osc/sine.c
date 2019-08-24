/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

struct sine {
	float freq;             /* base frequency */
	uint32_t x;             /* current x-value */
	uint32_t xstep;         /* current x-step */
};

/******************************************************************************
 * module port functions
 */

/* sine_port_reset resets the phase of the oscillator */
static void sine_port_reset(struct module *m, const struct event *e)
{
	bool reset = event_get_bool(e);

	if (reset) {
		struct sine *this = (struct sine *)m->priv;
		LOG_DBG("%s_%08x phase reset", m->info->name, m->id);
		/* start at a phase that gives a zero output */
		this->x = QuarterCycle;
	}
}

/* sine_port_frequency sets the frequency of the oscillator */
static void sine_port_frequency(struct module *m, const struct event *e)
{
	struct sine *this = (struct sine *)m->priv;
	float frequency = clampf_lo(event_get_float(e), 0);

	LOG_INF("set frequency %f Hz", frequency);
	this->freq = frequency;
	this->xstep = (uint32_t)(frequency * FrequencyScale);
}

/******************************************************************************
 * module functions
 */

static int sine_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct sine *this = ggm_calloc(1, sizeof(struct sine));

	if (this == NULL) {
		return -1;
	}

	m->priv = (void *)this;

	/* start at a phase that gives a zero output */
	this->x = QuarterCycle;
	return 0;
}

static void sine_free(struct module *m)
{
	ggm_free(m->priv);
}

static bool sine_process(struct module *m, float *buf[])
{
	struct sine *this = (struct sine *)m->priv;
	float *out = buf[0];

	for (int i = 0; i < AudioBufferSize; i++) {
		out[i] = cos_lookup(this->x);
		this->x += this->xstep;
	}

	return true;
}

#if 0

{ .name = "am", .type = PORT_TYPE_AUDIO, },
{ .name = "fm", .type = PORT_TYPE_AUDIO, },
{ .name = "pm", .type = PORT_TYPE_AUDIO, },

/* phase modulation */
if (pm != NULL) {
	for (int i = 0; i < AudioBufferSize; i++) {
		out[i] = cos_lookup(this->x);
		this->x += (uint32_t)((float)this->xstep + (pm[i] * PhaseScale));
	}
}

/* frequency modulation */
if (fm != NULL) {
	for (int i = 0; i < AudioBufferSize; i++) {
		out[i] = cos_lookup(this->x);
		this->x += (uint32_t)((this->freq + fm[i]) * FrequencyScale);
	}
}

/* amplitude modulation */
if (am != NULL) {
	block_mul(out, am);
}

#endif

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "reset", .type = PORT_TYPE_BOOL, .func = sine_port_reset },
	{ .name = "frequency", .type = PORT_TYPE_FLOAT, .func = sine_port_frequency },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info osc_sine_module = {
	.name = "osc.sine",
	.in = in_ports,
	.out = out_ports,
	.alloc = sine_alloc,
	.free = sine_free,
	.process = sine_process,
};

MODULE_REGISTER(osc_sine_module);

/*****************************************************************************/
