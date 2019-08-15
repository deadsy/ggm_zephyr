/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"
#include "seq/seq.h"

/******************************************************************************
 * patterns
 */

#define MIDI_CHANNEL 1

/* 4/4 signature */
static const uint8_t signature_4_4[] = {
	SEQ_OP_NOTE, MIDI_CHANNEL, 69, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CHANNEL, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CHANNEL, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_NOTE, MIDI_CHANNEL, 60, 100, 4,
	SEQ_OP_REST, 12,
	SEQ_OP_LOOP,
};

/******************************************************************************
 * private state
 */

struct metro {
	struct module *seq;     /* sequencer */
	struct module *mon;     /* MIDI monitor */
};

/******************************************************************************
 * module port functions
 */

static void metro_port_midi(struct module *m, const struct event *e)
{
	/* TODO process a CC for bpm control, etc. */
}

/******************************************************************************
 * module functions
 */

static int metro_alloc(struct module *m, va_list vargs)
{
	struct module *seq = NULL;
	struct module *mon = NULL;

	LOG_MOD_NAME(m);

	/* allocate the private data */
	struct metro *this = ggm_calloc(1, sizeof(struct metro));
	if (this == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}
	m->priv = (void *)this;

	/* sequencer */
	seq = module_new(m->top, "seq.seq", signature_4_4);
	if (seq == NULL) {
		LOG_ERR("could not create sequencer");
		goto error;
	}
	event_in_float(seq, "bpm", 120.0f, NULL);
	event_in_int(seq, "ctrl", SEQ_CTRL_START, NULL);
	this->seq = seq;

	/* midi monitor */
	mon = module_new(m->top, "midi.mon", MIDI_CHANNEL);
	if (mon == NULL) {
		LOG_ERR("could not create MIDI monitor");
		goto error;
	}
	this->mon = mon;

	/* monitor the sequencer output */
	port_connect(seq, "midi", mon, "midi");
	/* forward the sequencer output to the metronome output */
	port_forward(seq, "midi", m, "midi");

	return 0;

error:
	module_del(seq);
	module_del(mon);
	ggm_free(this);
	return -1;
}

static void metro_free(struct module *m)
{
	struct metro *this = (struct metro *)m->priv;

	LOG_MOD_NAME(m);
	module_del(this->seq);
	module_del(this->mon);
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
	PORT_EOL,
};

const struct module_info seq_metro_module = {
	.name = "seq.metro",
	.in = in_ports,
	.out = out_ports,
	.alloc = metro_alloc,
	.free = metro_free,
	.process = metro_process,
};

MODULE_REGISTER(seq_metro_module);

/*****************************************************************************/
