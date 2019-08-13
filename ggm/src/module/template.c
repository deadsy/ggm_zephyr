/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

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

static int xmod_alloc(struct module *m, va_list vargs)
{
	LOG_MOD_NAME(m);

	/* allocate the private data */
	struct xmod *this = ggm_calloc(1, sizeof(struct xmod));
	if (this == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}
	m->priv = (void *)this;

	return 0;
}

static void xmod_free(struct module *m)
{
	struct xmod *this = (struct xmod *)m->priv;

	LOG_MOD_NAME(m);
	ggm_free(this);
}

static bool xmod_process(struct module *m, float *buf[])
{
	struct xmod *this = (struct xmod *)m->priv;
	float *out = buf[0];

	(void)this;
	(void)out;

	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "name", .type = PORT_TYPE_FLOAT, .func = xmod_port_name },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info xmod_module = {
	.name = "xmod",
	.in = in_ports,
	.out = out_ports,
	.alloc = xmod_alloc,
	.free = xmod_free,
	.process = xmod_process,
};

MODULE_REGISTER(xmod_module);

/*****************************************************************************/
