/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Module to generate breath excitation for wind instruments.
 */

#include "ggm.h"
#include "osc/osc.h"

/******************************************************************************
 * private state
 */

struct breath {
	struct module *noise;   /* noise module */
	struct module *adsr;    /* adsr module */
	float k;                /* noise scalar */
};

/******************************************************************************
 * module port functions
 */

static void breath_port_reset(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "reset", e, NULL);
}

static void breath_port_gate(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "gate", e, NULL);
}

static void breath_port_attack(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "attack", e, NULL);
}

static void breath_port_decay(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "decay", e, NULL);
}

static void breath_port_sustain(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "sustain", e, NULL);
}

static void breath_port_release(struct module *m, const struct event *e)
{
	struct breath *this = (struct breath *)m->priv;

	event_in(this->adsr, "release", e, NULL);
}

/******************************************************************************
 * module functions
 */

static int breath_alloc(struct module *m, va_list vargs)
{
	struct module *noise = NULL;
	struct module *adsr = NULL;

	/* allocate the private data */
	struct breath *this = ggm_calloc(1, sizeof(struct breath));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* noise */
	noise = module_new(m->top, "osc.noise", NOISE_TYPE_WHITE);
	if (noise == NULL) {
		goto error;
	}
	this->noise = noise;

	/* adsr */
	adsr = module_new(m->top, "env.adsr");
	if (adsr == NULL) {
		goto error;
	}
	event_in_float(adsr, "attack", 0.1f, NULL);
	event_in_float(adsr, "decay", 0.5f, NULL);
	event_in_float(adsr, "sustain", 0.85f, NULL);
	event_in_float(adsr, "release", 1.f, NULL);
	this->adsr = adsr;

	return 0;

error:
	module_del(noise);
	module_del(adsr);
	ggm_free(m->priv);
	return -1;
}

static void breath_free(struct module *m)
{
	struct breath *this = (struct breath *)m->priv;

	module_del(this->noise);
	module_del(this->adsr);
	ggm_free(this);
}

static bool breath_process(struct module *m, float *bufs[])
{
	struct breath *this = (struct breath *)m->priv;
	struct module *adsr = this->adsr;
	float env[AudioBufferSize];
	bool active = adsr->info->process(adsr, (float *[]){ env, });

	if (active) {
		struct module *noise = this->noise;
		float *out = bufs[0];
		noise->info->process(noise, (float *[]){ out, });
		block_mul(out, env);
		block_mul_k(out, this->k);
		block_add(out, env);
	}

	return active;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "reset", .type = PORT_TYPE_BOOL, .func = breath_port_reset },
	{ .name = "gate", .type = PORT_TYPE_FLOAT, .func = breath_port_gate },
	{ .name = "attack", .type = PORT_TYPE_FLOAT, .func = breath_port_attack },
	{ .name = "decay", .type = PORT_TYPE_FLOAT, .func = breath_port_decay },
	{ .name = "sustain", .type = PORT_TYPE_FLOAT, .func = breath_port_sustain },
	{ .name = "release", .type = PORT_TYPE_FLOAT, .func = breath_port_release },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info pm_breath_module = {
	.name = "pm.breath",
	.in = in_ports,
	.out = out_ports,
	.alloc = breath_alloc,
	.free = breath_free,
	.process = breath_process,
};

MODULE_REGISTER(pm_breath_module);

/*****************************************************************************/
