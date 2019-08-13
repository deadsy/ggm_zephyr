/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * the set of all registered modules
 */

extern struct module_info sine_module;
extern struct module_info adsr_module;
extern struct module_info osc_voice_module;
extern struct module_info seq_module;
extern struct module_info metro_module;
extern struct module_info midi_mon_module;

/* module_list is a list off all the system modules */
static const struct module_info *module_list[] = {
	&sine_module,
	&adsr_module,
	&osc_voice_module,
	&seq_module,
	&metro_module,
	&midi_mon_module,
	NULL,
};

/* module_find finds a module by name */
static const struct module_info *module_find(const char *name)
{
	const struct module_info *mi;
	int i = 0;

	while ((mi = module_list[i]) != NULL) {
		if (strcmp(mi->name, name) == 0) {
			return mi;
		}
		i++;
	}
	return NULL;
}

/******************************************************************************
 * module new and free
 */

static uint32_t g_module_id;

/* get_module_id returns a unique identifier for each allocated module */
static uint32_t get_module_id(void)
{
	if (g_module_id == 0) {
		g_module_id++;
	}
	uint32_t id = g_module_id;
	g_module_id++;
	return id;
}

/* module_new returns a new instance of a module. */
struct module *module_new(struct synth *top, const char *name, ...)
{
	va_list vargs;

	/* find the module */
	const struct module_info *mi = module_find(name);

	if (mi == NULL) {
		LOG_ERR("could not find module \"%s\"", name);
		return NULL;
	}

	/* allocate the module */
	struct module *m = ggm_calloc(1, sizeof(struct module));
	if (m == NULL) {
		LOG_ERR("could not allocate module");
		return NULL;
	}

	/* fill in the module data */
	m->id = get_module_id();
	m->top = top;
	m->info = mi;

	/* allocate link list headers for the output port destinations */
	int n = port_count(mi->out);
	if (n > 0) {
		struct output_dst **dst = ggm_calloc(n, sizeof(void *));
		if (dst == NULL) {
			LOG_ERR("could not allocate output destination lists");
			goto error;
		}
		m->dst = dst;
	}

	/* allocate and initialise the module private data */
	va_start(vargs, name);
	int err = mi->alloc(m, vargs);
	va_end(vargs);
	if (err != 0) {
		LOG_ERR("could not initialise module");
		goto error;
	}

	return m;

error:
	ggm_free(m->dst);
	ggm_free(m);
	return NULL;
}

void module_del(struct module *m)
{
	if (m == NULL) {
		return;
	}

	/* free the private data */
	m->info->free(m);

	/* deallocate the lists of output destinations */
	int n = port_count(m->info->out);
	for (int i = 0; i < n; i++) {
		port_free_dst_list(m->dst[i]);
	}

	/* deallocate the headers */
	ggm_free(m->dst);

	/* deallocate the module data */
	ggm_free(m);
}

/*****************************************************************************/
