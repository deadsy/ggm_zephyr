/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * private state
 */

enum {
	SEQ_STATE_STOP, /* initial state */
	SEQ_STATE_RUN,
};

enum {
	OP_STATE_INIT, /* initial state */
	OP_STATE_WAIT,
};

enum {
	OP_NOP,
	OP_LOOP,
	OP_REST,
	OP_NOTE,
	OP_END,
};

struct seq_state_machine {
	uint32_t *prog;         /* program operations */
	int pc;                 /* program counter */
	int seq_state;          /* sequencer state */
	int op_state;           /* operation state */
	int duration;           /* operation duration */
};

struct basic_seq {
	float secs_per_tick;            /* seconds per tick */
	float tick_error;               /* current tick error*/
	uint32_t ticks;                 /* full ticks*/
	struct seq_state_machine *sm;   /* state machine */
};

/******************************************************************************
 * state machine operations
 */

static int prog_len(uint32_t *p)
{
	int i = 0;

	while (p[i] != OP_END) {
		i += 1;
	}
	return i;
}

static void tick(struct basic_seq *s, struct seq_state_machine *sm)
{
	/* auto stop zero length programs */
	if (sm->prog == NULL || prog_len(sm->prog) == 0) {
		sm->seq_state = SEQ_STATE_STOP;
	}
	/* run the program */
	if (sm->seq_state == SEQ_STATE_RUN) {

		int op = sm->prog[sm->pc];
		int n = 0;

		switch (op) {

		case OP_NOP:
			/* do nothing */
			n = 1;
			break;

		case OP_LOOP:
			/* loop back to the start of the program */
			sm->pc = -1;
			n = 1;
			break;

		default:
			LOG_ERR("unknown operation");
			n = 1;
			break;
		}

		sm->pc += n;
	}
}

/******************************************************************************
 * module port functions
 */

static void basic_seq_port_bpm(struct module *m, const struct event *e)
{
}

static void basic_seq_port_ctrl(struct module *m, const struct event *e)
{
}

/******************************************************************************
 * module functions
 */

static int basic_seq_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct basic_seq *x = k_calloc(1, sizeof(struct basic_seq));

	if (x == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}

	m->priv = (void *)x;
	return 0;
}

static void basic_seq_free(struct module *m)
{
	LOG_MOD_NAME(m);
	k_free(m->priv);
}

static bool basic_seq_process(struct module *m, float *buf[])
{
	struct basic_seq *this = (struct basic_seq *)m->priv;

	/* This routine is being used as a periodic call for timed event generation.
	 * The sequencer does not process audio buffers.
	 * The desired BPM will generally not correspond to an integral number
	 * of audio blocks, so accumulate an error and tick when needed.
	 * ie- Bresenham style.
	 */

	this->tick_error += SecsPerAudioBuffer;
	if (this->tick_error > this->secs_per_tick) {
		this->tick_error -= this->secs_per_tick;
		this->ticks++;
		/* tick the state machine */
		tick(this, this->sm);
	}
	return false;
}

/******************************************************************************
 * module information
 */

const static struct port_info in_ports[] = {
	{ .name = "bpm", .type = PORT_TYPE_FLOAT, .func = basic_seq_port_bpm },
	{ .name = "ctrl", .type = PORT_TYPE_INT, .func = basic_seq_port_ctrl },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, },
	PORT_EOL,
};

const struct module_info basic_seq_module = {
	.name = "basic_seq",
	.in = in_ports,
	.out = out_ports,
	.alloc = basic_seq_alloc,
	.free = basic_seq_free,
	.process = basic_seq_process,
};

MODULE_REGISTER(basic_seq_module);

/*****************************************************************************/
