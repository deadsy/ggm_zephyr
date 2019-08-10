/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_PORT_H
#define GGM_SRC_INC_PORT_H

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

/* port_info contains the information describing a port */
struct port_info {
	const char *name;       /* port name */
	enum port_type type;    /* port type */
	port_func func;         /* port event function */
};

#define PORT_EOL { NULL, PORT_TYPE_NULL, NULL }

/******************************************************************************
 * function prototypes
 */

int port_num_by_name(const struct port_info port[], const char *name);
void port_connect(struct module *s, const char *sname, struct module *d, const char *dname);

/*****************************************************************************/

#endif /* GGM_SRC_INC_PORT_H */

/*****************************************************************************/
