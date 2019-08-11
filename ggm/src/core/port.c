/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * port_num_by_name returns the number of ports within a set matching a name.
 */

#if 0

int port_num_by_name(const struct port_info port[], const char *name)
{
	int n = 0;
	int i = 0;

	while (port[i].type != PORT_TYPE_NULL) {
		if (strcmp(name, port[i].name) == 0) {
			n += 1;
		}
		i++;
	}

	return n;
}

#endif

/******************************************************************************
 * port lookup functions
 */

/* port_get_index returns the array index of a named port */
int port_get_index(const struct port_info port[], const char *name)
{
	int i = 0;

	while (port[i].type != PORT_TYPE_NULL) {
		if (strcmp(name, port[i].name) == 0) {
			return i;
		}
		i++;
	}
	return -1;
}

/* port_get_info returns the port info of a named port */
const struct port_info *port_get_info(const struct port_info port[], const char *name)
{
	int i = port_get_index(port, name);

	return (i >= 0) ? &port[i] : NULL;
}

/******************************************************************************
 * output port connection functions
 */

/* port_connect connects an output port to an input port */
void port_connect(struct module *s, const char *sname, struct module *d, const char *dname)
{
	const struct module_info *si = s->info;
	const struct module_info *di = d->info;

	/* does the output port exist on the source module? */
	if (port_get_index(si->out, sname) < 0) {
		LOG_ERR("module \"%s\" must have an output port named \"%s\"", si->name, sname);
	}

	/* does the input port exist on the destination module? */
	if (port_get_index(di->in, dname) < 0) {
		LOG_ERR("module \"%s\" must have an input port named \"%s\"", di->name, dname);
	}
}

/* port_connect_thru connects an output port to an output port */
void port_connect_thru(struct module *s, const char *sname, struct module *d, const char *dname)
{
	const struct module_info *si = s->info;
	const struct module_info *di = d->info;

	/* does the output port exist on the source module? */
	if (port_get_index(si->out, sname) < 0) {
		LOG_ERR("module \"%s\" must have an output port named \"%s\"", si->name, sname);
	}

	/* does the output port exist on the destination module? */
	if (port_get_index(di->out, dname) < 0) {
		LOG_ERR("module \"%s\" must have an ouput port named \"%s\"", di->name, dname);
	}

}

/*****************************************************************************/
