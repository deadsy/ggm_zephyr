/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Oscillator Voice
 * This voice is a generic oscillator with an ADSR envelope applied to it.
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

struct osc_voice {
	struct module *adsr;    /* adsr envelope */
	struct module *osc;     /* oscillator */
	port_func gate;         /* port function cache */
	port_func freq;         /* port function cache */
};

/******************************************************************************
 * module port functions
 */

static void osc_voice_gate(struct module *m, const struct event *e)
{
	struct osc_voice *this = (struct osc_voice *)m->priv;

	event_in(this->adsr, "gate", e, &this->gate);
}

static void osc_voice_note(struct module *m, const struct event *e)
{
	struct osc_voice *this = (struct osc_voice *)m->priv;
	float f = midi_to_frequency(event_get_float(e));

	event_in_float(this->osc, "frequency", f, &this->freq);
}

/******************************************************************************
 * module functions
 */

static int osc_voice_alloc(struct module *m, va_list vargs)
{
	struct module *osc = NULL;
	struct module *adsr = NULL;

	/* allocate the private data */
	struct osc_voice *this = k_calloc(1, sizeof(struct osc_voice));

	if (this == NULL) {
		LOG_ERR("could not allocate private data");
		goto error;
	}
	m->priv = (void *)this;

	/* oscillator */
	module_func new_osc = va_arg(vargs, module_func);
	osc = new_osc(m->top);
	if (osc == NULL) {
		LOG_ERR("could not create oscillator module");
		goto error;
	}
	event_in_float(osc, "duty", 0.1f, NULL);
	event_in_float(osc, "attenuation", 1.f, NULL);
	event_in_float(osc, "slope", 0.5f, NULL);
	this->osc = osc;

	/* adsr */
	adsr = module_new(m->top, "adsr");
	if (adsr == NULL) {
		LOG_ERR("could not create adsr module");
		goto error;
	}
	event_in_float(adsr, "attack", 0.1f, NULL);
	event_in_float(adsr, "decay", 0.5f, NULL);
	event_in_float(adsr, "sustain", 0.05f, NULL);
	event_in_float(adsr, "release", 1.f, NULL);
	this->adsr = adsr;

	return 0;

error:
	module_del(osc);
	module_del(adsr);
	k_free(m->priv);
	return -1;
}

static void osc_voice_free(struct module *m)
{
	LOG_MOD_NAME(m);

	struct osc_voice *this = (struct osc_voice *)m->priv;

	module_del(this->osc);
	module_del(this->adsr);
	k_free(m->priv);
}

static bool osc_voice_process(struct module *m, float *buf[])
{
	struct osc_voice *this = (struct osc_voice *)m->priv;
	float *out = buf[0];

	(void)this;
	(void)out;

	return true;
}

/******************************************************************************
 * module information
 */

const static struct port_info in_ports[] = {
	{ .name = "gate", .type = PORT_TYPE_FLOAT, .func = osc_voice_gate },
	{ .name = "note", .type = PORT_TYPE_FLOAT, .func = osc_voice_note },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info osc_voice_module = {
	.name = "osc_voice",
	.in = in_ports,
	.out = out_ports,
	.alloc = osc_voice_alloc,
	.free = osc_voice_free,
	.process = osc_voice_process,
};

MODULE_REGISTER(osc_voice_module);

/*****************************************************************************/
