/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_EVENT_H
#define GGM_SRC_INC_EVENT_H

/******************************************************************************
 * events
 */

struct event {
	union {
		float fval;
		int32_t ival;
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

static inline float event_get_float(struct event e)
{
	return e.u.fval;
}

static inline struct event event_set_float(float x)
{
	struct event e;

	e.u.fval = x;
	return e;
}

/******************************************************************************
 * integer events
 */

static inline float event_get_int(struct event e)
{
	return e.u.ival;
}

/******************************************************************************
 * boolean events
 */

static inline float event_get_bool(struct event e)
{
	return e.u.bval;
}

#endif /* GGM_SRC_INC_EVENT_H */
