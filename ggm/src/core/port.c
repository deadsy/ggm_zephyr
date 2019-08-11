/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * port counting functions
 */

/* port_count return the number of ports on an input/output list */
int port_count(const struct port_info port[])
{
	int i = 0;

	if (port == NULL) {
		return 0;
	}
	while (port[i].type != PORT_TYPE_NULL) {
		i++;
	}
	return i;
}

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
 * output destination list functions
 */

/* port_add_dst adds a destination port to the output */
static void port_add_dst(struct module *m, int idx, struct module *dst, port_func func)
{
}

/******************************************************************************
 * output port connection functions
 */

/* port_connect connects an output port to an input port */
void port_connect(struct module *s, const char *sname, struct module *d, const char *dname)
{
	const struct module_info *si = s->info;
	const struct module_info *di = d->info;

	LOG_INF("%s_%08x:%s to %s_%08x:%s", si->name, s->id, sname,  di->name, d->id, dname);

	/* does the output port exist on the source module? */
	int s_idx = port_get_index(si->out, sname);
	if (s_idx < 0) {
		LOG_ERR("output port does not exist");
		return;
	}

	/* does the input port exist on the destination module? */
	int d_idx = port_get_index(di->in, dname);
	if (d_idx < 0) {
		LOG_ERR("input port does not exist");
		return;
	}

	/* do the port type match? */
	int s_type = si->out[s_idx].type;
	int d_type = di->in[d_idx].type;
	if (s_type != d_type) {
		LOG_ERR("port types must be the same");
		return;
	}

	/* make sure we are not trying to connect audio ports */
	if (s_type == PORT_TYPE_AUDIO) {
		LOG_ERR("ports must be event ports (not audio)");
		return;
	}

	/* check for a non-null destination port function */
	port_func d_pf = di->in[d_idx].func;
	if (d_pf == NULL) {
		LOG_ERR("input port must have a port function");
		return;
	}

	port_add_dst(s, s_idx, d, d_pf);
}

/* port_connect_thru connects an output port to an output port */
void port_connect_thru(struct module *s, const char *sname, struct module *d, const char *dname)
{
	const struct module_info *si = s->info;
	const struct module_info *di = d->info;

	LOG_INF("%s_%08x:%s to %s_%08x:%s", si->name, s->id, sname,  di->name, d->id, dname);

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
