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
#include "seq.h"

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
	int (*alloc)(struct module *m, va_list vargs);          /* allocate and initialise the module */
	void (*free)(struct module *m);                         /* stop and deallocate the module */
	bool (*process)(struct module *m, float *buf[]);        /* process buffers for this module */
};

typedef struct module * (*module_func)(struct synth *s);

#define LOG_MOD_NAME(m) LOG_INF("%s_%08x", (m)->info->name, (m)->id)

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

#include "event.h"

/* port_info contains the information describing a port. */
struct port_info {
	const char *name;       /* port name */
	enum port_type type;    /* port type */
	port_func func;         /* port event function */
};

#define PORT_EOL { NULL, PORT_TYPE_NULL, NULL }

/******************************************************************************
 * function prototypes
 */

struct module *module_new(struct synth *top, const char *name, ...);
void module_del(struct module *m);

int port_num_by_name(const struct port_info port[], const char *name);
void port_connect(struct module *s, const char *sname, struct module *d, const char *dname);

struct synth *synth_new(void);
void synth_set_root(struct synth *s, struct module *m);
void synth_del(struct synth *s);

/*****************************************************************************/

#endif /* GGM_SRC_INC_MODULE_H */

/*****************************************************************************/
