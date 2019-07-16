/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/******************************************************************************
 * Oscillator Voice
 * This voice is a generic oscillator with an ADSR envelope applied to it.
 */

#include "module.h"

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

static int osc_voice_init(struct module *m, va_list vargs)
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
	osc = module_new(m->top, "sine");
	if (osc == NULL) {
		LOG_ERR("could not create sine module");
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
	module_free(osc);
	module_free(adsr);
	k_free(m->priv);
	return -1;
}

static void osc_voice_stop(struct module *m)
{
	k_free(m->priv);
}

static struct module **osc_voice_child(struct module *m)
{
	/* no children */
	return NULL;
}

static bool osc_voice_process(struct module *m, float *buf[])
{
	struct xmod_data *x = (struct xmod_data *)m->priv;
	float *out = buf[0];

	(void)x;
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
	.init = osc_voice_init,
	.stop = osc_voice_stop,
	.child = osc_voice_child,
	.process = osc_voice_process,
};

MODULE_REGISTER(osc_voice_module);

/*****************************************************************************/
