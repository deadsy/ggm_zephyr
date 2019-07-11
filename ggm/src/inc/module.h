/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_MODULE_H
#define GGM_SRC_INC_MODULE_H

#include <stddef.h>
#include <inttypes.h>

#include "util.h"
#include "event.h"

/******************************************************************************
 * module
 */

struct module_info {

	char *name;             /* module name */
	struct port_info *in;   /* input ports */
	struct port_info *out;  /* output ports */
};

struct module {
};

/******************************************************************************
 * module ports
 */

#define PORT_NAME(x) STRHASH(x)

enum port_type {
	PORT_TYPE_NULL = 0,
	PORT_TYPE_AUDIO,                /* audio buffers */
	PORT_TYPE_FLOAT,                /* event with float32 values */
	PORT_TYPE_INT,                  /* event with integer values */
	PORT_TYPE_BOOL,                 /* event with boolean values */
	PORT_TYPE_MIDI,                 /* event with MIDI data */
};

/* PortInfo contains the information describing a port. */
struct port_info {
  char *name; /* port name */
	uint32_t id;                                            /* port name hash */
	enum port_type type;                                    /* port type */
	void (*func)(struct module *m, struct event *e);        /* port event function */
};

#endif /* GGM_SRC_INC_MODULE_H */
