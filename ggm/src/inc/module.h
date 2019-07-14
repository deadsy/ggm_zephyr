/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_MODULE_H
#define GGM_SRC_INC_MODULE_H

#include <stddef.h>
#include <stdbool.h>
#include <inttypes.h>

#include <zephyr.h>

#include "ggm.h"
#include "util.h"
#include "event.h"

/******************************************************************************
 * module
 */

struct module {
	uint32_t id;                    /* module identifier */
	struct synth *top;              /* top level synth */
	const struct module_info *info; /* module info */
	void *priv;                     /* pointer to private module data */
};

struct module_info {
	char *name;                                             /* module name */
	const struct port_info *in;                             /* input ports */
	const struct port_info *out;                            /* output ports */
	int (*init)(struct module *m);                          /* initialise the module */
	void (*stop)(struct module *m);                         /* stop and deallocate the module */
	struct module ** (*child)(struct module *m);            /* return a list of child modules */
	bool (*process)(struct module *m, float *buf[]);        /* process buffers for this module */
};

#define MODULE_REGISTER(x)

/******************************************************************************
 * module ports
 */

enum port_type {
	PORT_TYPE_NULL = 0,
	PORT_TYPE_AUDIO,                /* audio buffers */
	PORT_TYPE_FLOAT,                /* event with float values */
	PORT_TYPE_INT,                  /* event with integer values */
	PORT_TYPE_BOOL,                 /* event with boolean values */
	PORT_TYPE_MIDI,                 /* event with MIDI data */
};

/* port_info contains the information describing a port. */
struct port_info {
	char *name;                                             /* port name */
	uint32_t id;                                            /* port name hash */
	enum port_type type;                                    /* port type */
	void (*func)(struct module *m, struct event *e);        /* port event function */
};

/******************************************************************************
 * function prototypes
 */

struct module *module_new(struct synth *top, char *name);
void module_free(struct module *m);
char *module_str(struct module *m, char *buf);

/*****************************************************************************/

#endif /* GGM_SRC_INC_MODULE_H */

/*****************************************************************************/
