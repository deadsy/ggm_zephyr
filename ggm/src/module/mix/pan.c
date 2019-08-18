/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Left/Right Pan and Volume Module
 * Takes a single audio buffer stream as input and outputs left and right channels.
 *
 * Arguments:
 * uint8_t ch, MIDI channel
 * uint8_t cc_pan, MIDI CC number for pan control
 * uint8_t cc_vol, MIDI CC number for volume control
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

struct pan {
	uint8_t ch;             /* MIDI channel */
	uint8_t cc_pan;         /* MIDI CC number for pan control */
	uint8_t cc_vol;         /* MIDI CC number for volume control */
	float vol;              /* overall volume */
	float pan;              /* pan value 0 == left, 1 == right */
	float vol_l;            /* left channel volume */
	float vol_r;            /* right channel volume */
};

/******************************************************************************
 * module port functions
 */

static void pan_set(struct module *m)
{
	struct pan *this = (struct pan *)m->priv;

	/* Use sin/cos so that l*l + r*r = K (constant power) */
	this->vol_l = this->vol * cosf(this->pan);
	this->vol_r = this->vol * sinf(this->pan);
}

static void set_vol(struct module *m, float vol)
{
	struct pan *this = (struct pan *)m->priv;

	LOG_DBG("set volume %f", vol);
	/* convert to a linear volume */
	this->vol = pow2(vol) - 1.f;
	pan_set(m);
}

static void set_pan(struct module *m, float pan)
{
	struct pan *this = (struct pan *)m->priv;

	LOG_DBG("set pan %f", pan);
	this->pan = pan * (0.5f * Pi);
	pan_set(m);
}

static void pan_port_midi(struct module *m, const struct event *e)
{
	struct pan *this = (struct pan *)m->priv;

	e =  event_filter_midi_channel(e, this->ch);
	if (e != NULL) {
		if (event_get_midi_msg(e) == MIDI_STATUS_CONTROLCHANGE) {
			uint8_t cc = event_get_midi_cc_num(e);
			if (cc == this->cc_pan) {
				set_pan(m, event_get_midi_cc_float(e));
			} else if (cc == this->cc_vol) {
				set_vol(m, event_get_midi_cc_float(e));
			}
		}
	}
}

static void pan_port_vol(struct module *m, const struct event *e)
{
	set_vol(m, clampf(event_get_float(e), 0.f, 1.f));
}

static void pan_port_pan(struct module *m, const struct event *e)
{
	set_pan(m, clampf(event_get_float(e), 0.f, 1.f));
}

/******************************************************************************
 * module functions
 */

static int pan_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct pan *this = ggm_calloc(1, sizeof(struct pan));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* setup the MIDI parameters */
	this->ch = va_arg(vargs, int);
	this->cc_pan = va_arg(vargs, int);
	this->cc_vol = va_arg(vargs, int);

	LOG_DBG("ch %d cc_pan %d cc_vol %d", this->ch, this->cc_pan, this->cc_vol);

	/* set some default values */
	set_vol(m, 1.f);
	set_pan(m, 0.5f);

	return 0;
}

static void pan_free(struct module *m)
{
	struct pan *this = (struct pan *)m->priv;

	ggm_free(this);
}

static bool pan_process(struct module *m, float *bufs[])
{
	struct pan *this = (struct pan *)m->priv;
	float *in = bufs[0];
	float *out0 = bufs[1];
	float *out1 = bufs[2];

	block_copy_mul_k(out0, in, this->vol_l);
	block_copy_mul_k(out1, in, this->vol_r);
	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "in", .type = PORT_TYPE_AUDIO, },
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = pan_port_midi, },
	{ .name = "vol", .type = PORT_TYPE_FLOAT, .func = pan_port_vol, },
	{ .name = "pan", .type = PORT_TYPE_FLOAT, .func = pan_port_pan, },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out0", .type = PORT_TYPE_AUDIO, },
	{ .name = "out1", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info mix_pan_module = {
	.name = "mix.pan",
	.in = in_ports,
	.out = out_ports,
	.alloc = pan_alloc,
	.free = pan_free,
	.process = pan_process,
};

MODULE_REGISTER(mix_pan_module);

/*****************************************************************************/
