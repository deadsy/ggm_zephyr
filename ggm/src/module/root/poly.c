/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"
#include "osc/osc.h"

/******************************************************************************
 * MIDI setup
 */

#define MIDI_CHAN 0

static const struct midi_cfg mcfg[] = {
	{ "root.poly.voice*.adsr:attack", MIDI_CHAN, 1 },
	{ "root.poly.voice*.adsr:decay", MIDI_CHAN, 2 },
	{ "root.poly.voice*.adsr:sustain", MIDI_CHAN, 3 },
	{ "root.poly.voice*.adsr:release", MIDI_CHAN, 4 },
	{ "root.pan:pan", MIDI_CHAN, 7 },
	{ "root.pan:vol", MIDI_CHAN, 8 },
	MIDI_CFG_EOL
};

/******************************************************************************
 * private state
 */

struct poly {
	struct module *poly;    /* polyphonic control */
	struct module *pan;     /* ouput left/right panning */
};

/******************************************************************************
 * polyphonic voice (module_func signature)
 */

struct module *voice_osc0(struct module *m, int id)
{
	return module_new(m, "osc/goom", id);
}

struct module *voice_osc1(struct module *m, int id)
{
	return module_new(m, "osc/noise", id, NOISE_TYPE_BROWN);
}

struct module *poly_voice0(struct module *m, int id)
{
	return module_new(m, "voice/osc", id, voice_osc0);
}

struct module *poly_voice1(struct module *m, int id)
{
	return module_new(m, "osc/ks", id);
}

/******************************************************************************
 * module port functions
 */

static void poly_port_midi(struct module *m, const struct event *e)
{
	struct poly *this = (struct poly *)m->priv;
	bool consumed = synth_midi_cc(m->top, e);

	/* did the synth level midi cc map dispatch the event? */
	if (consumed) {
		return;
	}

	char tmp[64];
	LOG_DBG("%s", log_strdup(midi_str(tmp, sizeof(tmp), e)));
	/* forward the MIDI events */
	event_in(this->poly, "midi", e, NULL);
}

/******************************************************************************
 * module functions
 */

static int poly_alloc(struct module *m, va_list vargs)
{
	struct module *poly = NULL;
	struct module *pan = NULL;

	/* allocate the private data */
	struct poly *this = ggm_calloc(1, sizeof(struct poly));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* Set the synth MIDI map.
	 * Do this before the sub-modules are created.
	 */
	int err = synth_set_midi_cfg(m->top, mcfg);
	if (err < 0) {
		goto error;
	}

	/* polyphony */
	poly = module_new(m, "midi/poly", -1, MIDI_CHAN, poly_voice0);
	if (poly == NULL) {
		goto error;
	}
	this->poly = poly;

	/* pan */
	pan = module_new(m, "mix/pan", -1);
	if (poly == NULL) {
		goto error;
	}
	this->pan = pan;

	return 0;

error:
	module_del(poly);
	module_del(pan);
	ggm_free(this);
	return -1;
}

static void poly_free(struct module *m)
{
	struct poly *this = (struct poly *)m->priv;

	module_del(this->poly);
	module_del(this->pan);
	ggm_free(this);
}

static bool poly_process(struct module *m, float *bufs[])
{
	struct poly *this = (struct poly *)m->priv;
	struct module *poly = this->poly;
	struct module *pan = this->pan;
	float *out0 = bufs[0];
	float *out1 = bufs[1];
	float tmp[AudioBufferSize];

	poly->info->process(poly, (float *[]){ tmp, });
	pan->info->process(pan, (float *[]){ tmp, out0, out1, });
	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = poly_port_midi },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out0", .type = PORT_TYPE_AUDIO },
	{ .name = "out1", .type = PORT_TYPE_AUDIO },
	PORT_EOL,
};

const struct module_info root_poly_module = {
	.mname = "root/poly",
	.iname = "root",
	.in = in_ports,
	.out = out_ports,
	.alloc = poly_alloc,
	.free = poly_free,
	.process = poly_process,
};

MODULE_REGISTER(root_poly_module);

/*****************************************************************************/
