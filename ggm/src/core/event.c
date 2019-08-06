/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * event_in sends an event to a named port on a module.
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

/******************************************************************************
 * event_out sends an event time event from the named output port of a module.
 * The event will be sent to input ports connected to the output port.
 */

void event_out(struct module *m, const char *name, const struct event *e)
{
}

/******************************************************************************
 * event_push sends a process time event from the named output port of a module.
 * The event will be sent to input ports connected to the output port.
 */

void event_push(struct module *m, const char *name, const struct event *e)
{
}

/******************************************************************************
 * event_connect connects source/destination module event ports.
 */

void event_connect(struct module *s, const char *sname, struct module *d, const char *dname)
{
	const struct module_info *si = s->info;
	const struct module_info *di = d->info;

	/* check output on source module*/
	if (port_num_by_name(si->out, sname) != 1) {
		LOG_ERR("module \"%s\" must have one output port named \"%s\"", si->name, sname);
	}

	/* check input on destination module*/
	if (port_num_by_name(di->in, dname) != 1) {
		LOG_ERR("module \"%s\" must have one input port named \"%s\"", di->name, dname);
	}




}

/*****************************************************************************/



