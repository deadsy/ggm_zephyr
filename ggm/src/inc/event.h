/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_EVENT_H
#define GGM_SRC_INC_EVENT_H

#ifndef GGM_SRC_INC_GGM_H
#warning "please include this file using ggm.h"
#endif

/******************************************************************************
 * events
 */

enum event_type {
	EVENT_TYPE_NULL = 0,
	EVENT_TYPE_FLOAT,       /* float value event */
	EVENT_TYPE_INT,         /* integer value event */
	EVENT_TYPE_BOOL,        /* boolean value event */
	EVENT_TYPE_MIDI,        /* MIDI event */
};

struct event {
	enum event_type type;
	union {
		float fval;
		int ival;
		bool bval;
		struct {
			uint8_t type;
			uint8_t status;
			uint8_t arg0;
			uint8_t arg1;
		} midi;
	} u;
};

/******************************************************************************
 * function prototypes
 */

typedef void (*port_func)(struct module *m, const struct event *e);

void event_in(struct module *m, const char *name, const struct event *e, port_func *hdl);
void event_out(struct module *m, const char *name, const struct event *e);
void event_push(struct module *m, const char *name, const struct event *e);

/******************************************************************************
 * MIDI events
 */

/* Channel Messages */
#define MIDI_STATUS_NOTEOFF (8 << 4)
#define MIDI_STATUS_NOTEON  (9 << 4)
#define MIDI_STATUS_POLYPHONICAFTERTOUCH  (10 << 4)
#define MIDI_STATUS_CONTROLCHANGE  (11 << 4)
#define MIDI_STATUS_PROGRAMCHANGE  (12 << 4)
#define MIDI_STATUS_CHANNELAFTERTOUCH  (13 << 4)
#define MIDI_STATUS_PITCHWHEEL  (14 << 4)

/* System Common Messages */
#define MIDI_STATUS_SYSEXSTART  0Xf0
#define MIDI_STATUS_QUARTERFRAME  0Xf1
#define MIDI_STATUS_SONGPOINTER  0Xf2
#define MIDI_STATUS_SONGSELECT  0Xf3
#define MIDI_STATUS_TUNEREQUEST  0Xf6
#define MIDI_STATUS_SYSEXEND  0Xf7

/* System Realtime Messages */
#define MIDI_STATUS_TIMINGCLOCK  0Xf8
#define MIDI_STATUS_START  0Xfa
#define MIDI_STATUS_CONTINUE  0Xfb
#define MIDI_STATUS_STOP  0Xfc
#define MIDI_STATUS_ACTIVESENSING  0Xfe
#define MIDI_STATUS_RESET  0Xff

/* Delimiters */
#define MIDI_STATUS_COMMON  0Xf0
#define MIDI_STATUS_REALTIME  0Xf8

/* MIDI event types */
#define MIDI_EVENT_NULL 0
#define MIDI_EVENT_NOTE_ON (MIDI_STATUS_NOTEON)
#define MIDI_EVENT_NOTE_OFF (MIDI_STATUS_NOTEOFF)
#define MIDI_EVENT_CONTROL_CHANGE (MIDI_STATUS_CONTROLCHANGE)
#define MIDI_EVENT_PITCH_WHEEL (MIDI_STATUS_PITCHWHEEL)
#define MIDI_EVENT_POLYPHONIC_AFTERTOUCH (MIDI_STATUS_POLYPHONICAFTERTOUCH)
#define MIDI_EVENT_PROGRAM_CHANGE (MIDI_STATUS_PROGRAMCHANGE)
#define MIDI_EVENT_CHANNEL_AFTERTOUCH (MIDI_STATUS_CHANNELAFTERTOUCH)

static inline void event_set_midi(struct event *e, uint8_t msg, uint8_t chan, uint8_t note, uint8_t velocity)
{
	e->type = EVENT_TYPE_MIDI;
	e->u.midi.type = 0;
	e->u.midi.status = 0;
	e->u.midi.arg0 = 0;
	e->u.midi.arg1 = 0;
}

/* event_get_midi_channel returns the MIDI channel number */
static uint8_t event_get_midi_channel(const struct event *e)
{
	return e->u.midi.status & 0xf;
}

static inline const struct event *event_filter_midi_channel(const struct event *e, uint8_t chan)
{
	if (e->type != EVENT_TYPE_MIDI) {
		return NULL;
	}
	if (event_get_midi_channel(e) != chan) {
		return NULL;
	}
	/* it's a MIDI event on the channel */
	return e;
}

char *midi_str(const struct event *e, char *s, size_t n);

/******************************************************************************
 * float events
 */

static inline void event_set_float(struct event *e, float x)
{
	e->type = EVENT_TYPE_FLOAT;
	e->u.fval = x;
}

static inline float event_get_float(const struct event *e)
{
	return e->u.fval;
}

static inline void event_in_float(struct module *m, const char *name, float val, port_func *hdl)
{
	struct event e;

	event_set_float(&e, val);
	event_in(m, name, &e, hdl);
}

/******************************************************************************
 * integer events
 */

static inline void event_set_int(struct event *e, int x)
{
	e->type = EVENT_TYPE_INT;
	e->u.ival = x;
}

static inline int event_get_int(const struct event *e)
{
	return e->u.ival;
}

static inline void event_in_int(struct module *m, const char *name, int val, port_func *hdl)
{
	struct event e;

	event_set_int(&e, val);
	event_in(m, name, &e, hdl);
}

/******************************************************************************
 * boolean events
 */

static inline bool event_get_bool(const struct event *e)
{
	return e->u.bval;
}

/*****************************************************************************/

#endif /* GGM_SRC_INC_EVENT_H */

/*****************************************************************************/
