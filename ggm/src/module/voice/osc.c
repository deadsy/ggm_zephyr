/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Oscillator Voice
 * This voice is a generic oscillator with an ADSR envelope applied to it.
 *
 * Arguments:
 * module_func func, function to create an oscillator module
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

struct osc {
	struct module *adsr;    /* adsr envelope */
	struct module *osc;     /* oscillator */
	port_func gate;         /* port function cache */
	port_func freq;         /* port function cache */
};

/******************************************************************************
 * module port functions
 */

static void osc_port_midi(struct module *m, const struct event *e)
{
	/* TODO */
}

static void osc_port_gate(struct module *m, const struct event *e)
{
	struct osc *this = (struct osc *)m->priv;

	event_in(this->adsr, "gate", e, &this->gate);
}

static void osc_port_note(struct module *m, const struct event *e)
{
	struct osc *this = (struct osc *)m->priv;
	float f = midi_to_frequency(event_get_float(e));

	event_in_float(this->osc, "frequency", f, &this->freq);
}

/******************************************************************************
 * module functions
 */

static int osc_alloc(struct module *m, va_list vargs)
{
	struct module *osc = NULL;
	struct module *adsr = NULL;

	/* allocate the private data */
	struct osc *this = ggm_calloc(1, sizeof(struct osc));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* oscillator */
	module_func new_osc = va_arg(vargs, module_func);
	osc = new_osc(m->top);
	if (osc == NULL) {
		goto error;
	}
	event_in_float(osc, "duty", 0.1f, NULL);
	event_in_float(osc, "attenuation", 1.f, NULL);
	event_in_float(osc, "slope", 0.5f, NULL);
	this->osc = osc;

	/* adsr */
	adsr = module_new(m->top, "env.adsr");
	if (adsr == NULL) {
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
	ggm_free(m->priv);
	return -1;
}

static void osc_free(struct module *m)
{
	struct osc *this = (struct osc *)m->priv;

	module_del(this->osc);
	module_del(this->adsr);
	ggm_free(m->priv);
}

static bool osc_process(struct module *m, float *buf[])
{
	struct osc *this = (struct osc *)m->priv;
	struct module *adsr = this->adsr;
	float env[AudioBufferSize];
	bool active = adsr->info->process(adsr, (float *[]){ env, });

	if (active) {
		struct module *osc = this->osc;
		float *out = buf[0];
		osc->info->process(osc, (float *[]){ out, });
		block_mul(out, env);
	}

	return active;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = osc_port_midi },
	{ .name = "gate", .type = PORT_TYPE_FLOAT, .func = osc_port_gate },
	{ .name = "note", .type = PORT_TYPE_FLOAT, .func = osc_port_note },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info voice_osc_module = {
	.name = "voice.osc",
	.in = in_ports,
	.out = out_ports,
	.alloc = osc_alloc,
	.free = osc_free,
	.process = osc_process,
};

MODULE_REGISTER(voice_osc_module);

/*****************************************************************************/
