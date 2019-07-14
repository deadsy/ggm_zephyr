/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_EVENT_H
#define GGM_SRC_INC_EVENT_H

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
 * float events
 */

static inline float event_get_float(const struct event *e)
{
	return e->u.fval;
}

static inline void event_set_float(struct event *e, float x)
{
	e->type = EVENT_TYPE_FLOAT;
	e->u.fval = x;
}

/******************************************************************************
 * integer events
 */

static inline int event_get_int(const struct event *e)
{
	return e->u.ival;
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
