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
#define MIDI_CC_PAN 7
#define MIDI_CC_VOL 8

/******************************************************************************
 * private state
 */

struct poly {
	struct module *poly;    /* polyphonic control */
	struct module *pan;     /* ouput left/right panning */
};

/******************************************************************************
 * polyphonic voice
 */

static struct module *voice_osc0(struct synth *top)
{
	return module_new(top, "osc.sine");
}

static struct module *voice_osc1(struct synth *top)
{
	return module_new(top, "osc.noise", NOISE_TYPE_BROWN);
}

static struct module *poly_voice(struct synth *top)
{
	return module_new(top, "voice.osc", voice_osc1);
}

/******************************************************************************
 * module port functions
 */

static void poly_port_midi(struct module *m, const struct event *e)
{
	struct poly *this = (struct poly *)m->priv;
	char tmp[64];

	LOG_DBG("%s", log_strdup(midi_str(tmp, sizeof(tmp), e)));

	/* forward the MIDI events */
	event_in(this->poly, "midi", e, NULL);
	event_in(this->pan, "midi", e, NULL);
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

	/* polyphony */
	poly = module_new(m->top, "midi.poly", MIDI_CHAN, poly_voice);
	if (poly == NULL) {
		goto error;
	}
	this->poly = poly;

	/* pan */
	pan = module_new(m->top, "mix.pan", MIDI_CHAN, MIDI_CC_PAN, MIDI_CC_VOL);
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
	.name = "root.poly",
	.in = in_ports,
	.out = out_ports,
	.alloc = poly_alloc,
	.free = poly_free,
	.process = poly_process,
};

MODULE_REGISTER(root_poly_module);

/*****************************************************************************/
