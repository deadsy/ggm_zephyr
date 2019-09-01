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
 * MIDI control mapping
 * The MIDI map is a synth-level table that maps a MIDI channel/control-change
 * number onto a module:port path. A given ch/cc can map to mutiple
 * module ports, the MIDI message will be sent to all of them. The module:port
 * path can have wildcards (*,?) for cases where multiple sub-modules of the
 * same type have been created. E.g. polyphony.
 */

struct midi_map {
	const char *path;       /* module:port path */
	uint8_t ch;             /* MIDI channel */
	uint8_t cc;             /* MIDI control number */
};

#define MIDI_MAP_EOL { NULL, 0, 0 }

/******************************************************************************
 * top-level synth structure
 */

#define NUM_EVENTS 16           /* must be a power of 2 */
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
	const struct midi_map *mmap;    /* MIDI map */
	float *bufs[NUM_AUDIO_PORTS];   /* allocated audio buffers */
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

int synth_set_midi_map(struct synth *s, const struct midi_map *map);
void synth_lookup_midi_map(struct synth *s, struct module *m, const struct port_info *pi);

/*****************************************************************************/

#endif /* GGM_SRC_INC_SYNTH_H */

/*****************************************************************************/
