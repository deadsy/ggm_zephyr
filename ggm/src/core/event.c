/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * event_in sends an event from a module to a port on a destination module.
 */

void event_in(struct module *m, const char *name, const struct event *e, port_func *hdl)
{
	const struct module_info *mi = m->info;
	port_func func = NULL;

	if (hdl == NULL || *hdl == NULL) {
		/* find the port function */
		int i = 0;
		while (1) {
			const struct port_info *p = &mi->in[i];
			if (p->type == PORT_TYPE_NULL) {
				/* end of list */
				break;
			}
			/* port name found ?*/
			if (strcmp(name, p->name) == 0) {
				func = p->func;
				break;
			}
			/*next ...*/
			i++;
		}
	} else {
		/* use the cached port function */
		func = *hdl;
	}

	if (func == NULL) {
		LOG_WRN("%s:%s not found", mi->name, name);
		return;
	}

	/* cache the function pointer */
	if (hdl && *hdl == NULL) {
		*hdl = func;
	}

	/* call the port function */
	func(m, e);
}

void event_in_float(struct module *m, const char *name, float val, port_func *hdl)
{
	struct event e;

	event_set_float(&e, val);
	event_in(m, name, &e, hdl);
}

/*****************************************************************************/
