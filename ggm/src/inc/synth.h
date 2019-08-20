/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_SYNTH_H
#define GGM_SRC_INC_SYNTH_H

#ifndef GGM_SRC_INC_GGM_H
#warning "please include this file using ggm.h"
#endif

/******************************************************************************
 * top-level synth structure
 */

#define NUM_EVENTS 16           /* must be a power of 2 */
#define NUM_MIDI_IN 4           /* max number of MIDI input ports for root patch */
#define NUM_AUDIO_PORTS 8       /* max number of audio input/output ports for root patch */

struct qevent {
	struct module *m;       /* source module */
	int idx;                /* output port index */
	struct event e;         /* the queued event */
};

/* circular buffer for events */
struct event_queue {
	struct qevent queue[NUM_EVENTS];
	size_t rd;
	size_t wr;
};

struct synth {
	struct module *root;            /* root patch */
	struct event_queue eq;          /* event queue */
	float *bufs[NUM_AUDIO_PORTS];   /* allocated audio buffers */
	port_func midi_in[NUM_MIDI_IN]; /* cache of MIDI input port functions */
	unsigned int n_audio_in;        /* number of input audio buffers */
	unsigned int n_audio_out;       /* number of output audio buffers */
	unsigned int n_midi_in;         /* number of input MIDI ports */
	unsigned int n_midi_out;        /* number of output MIDI ports */
};

/******************************************************************************
 * function prototypes
 */

struct synth *synth_new(void);
void synth_del(struct synth *s);
int synth_set_root(struct synth *s, struct module *m);
bool synth_has_root(struct synth *s);
void synth_loop(struct synth *s);
int synth_event_wr(struct synth *s, struct module *m, int idx, const struct event *e);
void synth_midi_in(struct synth *s, unsigned int idx, const struct event *e);

/*****************************************************************************/

#endif /* GGM_SRC_INC_SYNTH_H */

/*****************************************************************************/
