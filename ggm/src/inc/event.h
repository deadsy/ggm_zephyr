/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_EVENT_H
#define GGM_SRC_INC_EVENT_H

#include <assert.h>

/******************************************************************************
 * events
 */

enum event_type {
	EVENT_TYPE_NULL = 0,
	EVENT_TYPE_MIDI,                /* MIDI event */
	EVENT_TYPE_FLOAT,               /* float event */
	EVENT_TYPE_INT,                 /* int event */
	EVENT_TYPE_BOOL,                /* bool event */
};

struct event {
	enum event_type type;
	union {
		float val;
	} u;
};

static inline float event_get_float(struct event *e)
{
	assert(e->type == EVENT_TYPE_FLOAT);
	return e->u.val;
}

#endif /* GGM_SRC_INC_EVENT_H */
