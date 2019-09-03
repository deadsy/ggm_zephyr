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
	{ "root.seq:bpm", MIDI_CH, 8 },
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
};

/******************************************************************************
 * module port functions
 */

static void metro_port_midi(struct module *m, const struct event *e)
{
	synth_midi_cc(m->top, e);
}

/******************************************************************************
 * module functions
 */

static int metro_alloc(struct module *m, va_list vargs)
{
	struct module *seq = NULL;

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

	/* forward the sequencer output to the metronome output */
	port_forward(seq, "midi", m, "midi");

	return 0;

error:
	module_del(seq);
	ggm_free(this);
	return -1;
}

static void metro_free(struct module *m)
{
	struct metro *this = (struct metro *)m->priv;

	module_del(this->seq);
	ggm_free(this);
}

static bool metro_process(struct module *m, float *buf[])
{
	struct metro *this = (struct metro *)m->priv;
	struct module *seq = this->seq;

	seq->info->process(seq, NULL);
	return false;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = metro_port_midi },
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
