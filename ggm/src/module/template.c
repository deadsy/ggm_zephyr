/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * private state
 */

struct xmod {
};

/******************************************************************************
 * module port functions
 */

static void xmod_port_name(struct module *m, const struct event *e)
{
}

/******************************************************************************
 * module functions
 */

static int xmod_init(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct xmod *x = k_calloc(1, sizeof(struct xmod));

	if (x == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}

	m->priv = (void *)x;
	return 0;
}

static void xmod_stop(struct module *m)
{
	k_free(m->priv);
}

static struct module **xmod_child(struct module *m)
{
	/* no children */
	return NULL;
}

static bool xmod_process(struct module *m, float *buf[])
{
	struct xmod_data *x = (struct xmod_data *)m->priv;
	float *out = buf[0];

	(void)x;
	(void)out;

	return true;
}

/******************************************************************************
 * module information
 */

const static struct port_info in_ports[] = {
	{ .name = "name", .type = PORT_TYPE_FLOAT, .func = xmod_port_name },
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
};

const struct module_info xmod_module = {
	.name = "xmod",
	.in = in_ports,
	.out = out_ports,
	.init = xmod_init,
	.stop = xmod_stop,
	.child = xmod_child,
	.process = xmod_process,
};

MODULE_REGISTER(xmod_module);

/*****************************************************************************/
