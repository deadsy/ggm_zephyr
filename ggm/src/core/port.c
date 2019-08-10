/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * port_num_by_name returns the number of ports within a set matching a name.
 */

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


/******************************************************************************
 * port_connect connects source/destination module event ports.
 */

void port_connect(struct module *s, const char *sname, struct module *d, const char *dname)
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
