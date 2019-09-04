/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * This is the root patch for a metronome.
 *
 */

#include "ggm.h"
#include "seq/seq.h"


/******************************************************************************
 * MIDI setup
 */

#define MIDI_CH 0

static const struct midi_cfg mcfg[] = {
	{ "root.seq:bpm", MIDI_CH, 7 },
	{ "root.pan:vol", MIDI_CH, 8 },
	MIDI_CFG_EOL
};

/******************************************************************************
 * patterns
 */

/* 4/4 signature */
static const uint8_t signature_4_4[] = {
	SEQ_OP_NOTE, MIDI_CH, 69, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CH, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CH, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CH, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_LOOP,
};

/******************************************************************************
 * private state
 */

struct metro {
	struct module *seq;     /* sequencer */
	struct module *voice;   /* voice for audio output */
	struct module *pan;     /* mix mono to stereo output */
};

static struct module *voice_osc0(struct module *m, int id)
{
	return module_new(m, "osc/sine", id);
}

/******************************************************************************
 * module port functions
 */

static void metro_port_midi(struct module *m, const struct event *e)
{
	struct metro *this = (struct metro *)m->priv;
	bool consumed = synth_midi_cc(m->top, e);

	/* did the synth level midi cc map dispatch the event? */
	if (consumed) {
		return;
	}

	if (!is_midi_ch(e, MIDI_CH)) {
		return;
	}

	switch (event_get_midi_msg(e)) {

	case MIDI_STATUS_NOTEON: {
		uint8_t note = event_get_midi_note(e);
		float vel = event_get_midi_velocity_float(e);
		event_in_float(this->voice, "note", (float)note, NULL);
		event_in_float(this->voice, "gate", vel, NULL);
		break;
	}

	case MIDI_STATUS_NOTEOFF: {
		event_in_float(this->voice, "gate", 0.f, NULL);
		break;
	}

	}
}

/******************************************************************************
 * module functions
 */

static int metro_alloc(struct module *m, va_list vargs)
{
	struct module *seq = NULL;
	struct module *voice = NULL;
	struct module *pan = NULL;

	/* allocate the private data */
	struct metro *this = ggm_calloc(1, sizeof(struct metro));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* Set the synth MIDI map */
	int err = synth_set_midi_cfg(m->top, mcfg);
	if (err < 0) {
		goto error;
	}

	/* sequencer */
	seq = module_new(m, "seq/seq", -1, signature_4_4);
	if (seq == NULL) {
		goto error;
	}
	event_in_float(seq, "bpm", 120.f, NULL);
	event_in_int(seq, "ctrl", SEQ_CTRL_START, NULL);
	this->seq = seq;

	/* voice */
	voice = module_new(m, "voice/osc", -1, voice_osc0);
	if (voice == NULL) {
		goto error;
	}
	this->voice = voice;

	/* pan */
	pan = module_new(m, "mix/pan", -1);
	if (pan == NULL) {
		goto error;
	}
	this->pan = pan;

	/* forward the sequencer MIDI output to the MIDI output */
	port_forward(seq, "midi", m, "midi");

	/* connect the sequencer MIDI output to the MIDI input */
	port_connect(seq, "midi", m, "midi");

	return 0;

error:
	module_del(seq);
	module_del(voice);
	module_del(pan);
	ggm_free(this);
	return -1;
}

static void metro_free(struct module *m)
{
	struct metro *this = (struct metro *)m->priv;

	module_del(this->seq);
	module_del(this->voice);
	module_del(this->pan);
	ggm_free(this);
}

static bool metro_process(struct module *m, float *bufs[])
{
	struct metro *this = (struct metro *)m->priv;
	struct module *seq = this->seq;
	struct module *voice = this->voice;
	struct module *pan = this->pan;
	float *out0 = bufs[0];
	float *out1 = bufs[1];
	float tmp[AudioBufferSize];

	seq->info->process(seq, NULL);
	voice->info->process(voice, (float *[]){ tmp, });
	pan->info->process(pan, (float *[]){ tmp, out0, out1, });
	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .pf = metro_port_midi },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, },
	{ .name = "out0", .type = PORT_TYPE_AUDIO },
	{ .name = "out1", .type = PORT_TYPE_AUDIO },
	PORT_EOL,
};

const struct module_info root_metro_module = {
	.mname = "root/metro",
	.iname = "root",
	.in = in_ports,
	.out = out_ports,
	.alloc = metro_alloc,
	.free = metro_free,
	.process = metro_process,
};

MODULE_REGISTER(root_metro_module);

/*****************************************************************************/
