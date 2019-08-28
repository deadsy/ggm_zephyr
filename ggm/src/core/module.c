/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * the set of all registered modules
 */

extern struct module_info delay_delay_module;
extern struct module_info env_adsr_module;
extern struct module_info filter_svf_module;
extern struct module_info midi_mon_module;
extern struct module_info midi_poly_module;
extern struct module_info mix_pan_module;
extern struct module_info osc_goom_module;
extern struct module_info osc_ks_module;
extern struct module_info osc_lfo_module;
extern struct module_info osc_noise_module;
extern struct module_info osc_sine_module;
extern struct module_info root_metro_module;
extern struct module_info root_poly_module;
extern struct module_info seq_seq_module;
extern struct module_info voice_osc_module;
#if defined(__LINUX__)
extern struct module_info view_plot_module;
#endif

/* module_list is a list off all the system modules */
static const struct module_info *module_list[] = {
	&delay_delay_module,
	&env_adsr_module,
	&filter_svf_module,
	&midi_mon_module,
	&midi_poly_module,
	&mix_pan_module,
	&osc_goom_module,
	&osc_ks_module,
	&osc_lfo_module,
	&osc_noise_module,
	&osc_sine_module,
	&root_metro_module,
	&root_poly_module,
	&seq_seq_module,
	&voice_osc_module,
#if defined(__LINUX__)
	&view_plot_module,
#endif
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
		LOG_ERR("could not find module %s", name);
		return NULL;
	}

	/* allocate the module */
	struct module *m = ggm_calloc(1, sizeof(struct module));
	if (m == NULL) {
		goto error;
	}

	/* fill in the module data */
	m->id = get_module_id();
	m->top = top;
	m->info = mi;

	LOG_INF("%s_%08x", m->info->name, m->id);

	/* allocate link list headers for the output port destinations */
	int n = port_count(mi->out);
	if (n > 0) {
		struct output_dst **dst = ggm_calloc(n, sizeof(void *));
		if (dst == NULL) {
			goto error;
		}
		m->dst = dst;
	}

	/* allocate and initialise the module private data */
	va_start(vargs, name);
	int err = mi->alloc(m, vargs);
	va_end(vargs);
	if (err != 0) {
		goto error;
	}

	return m;

error:
	LOG_ERR("could not create module %s", name);
	if (m != NULL) {
		ggm_free(m->dst);
		ggm_free(m);
	}
	return NULL;
}

void module_del(struct module *m)
{
	if (m == NULL) {
		return;
	}

	LOG_INF("%s_%08x", m->info->name, m->id);

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
