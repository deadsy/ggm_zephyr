/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Plotting Module
 *
 * When this module is triggered it writes the input signal to python code
 * that uses the plot.ly library to create a plot of the signal.
 */

#include "ggm.h"
#include "view/view.h"

#if !defined(__LINUX__)
#error "Sorry, this module only works with Linux."
#endif

/******************************************************************************
 * private state
 */

struct plot {
	struct plot_cfg *cfg;   /* plot configuration */
	uint32_t x;             /* current x-value */
	uint32_t samples;       /* number of samples to plot per trigger */
	uint32_t samples_left;  /* samples left in this trigger */
	uint32_t idx;           /* file index number */
	bool triggered;         /* are we currently triggered? */
	FILE *f;                /* output file */
};

/******************************************************************************
 * plotting functions
 */

#define PLOT_HEADER		   \
	"#!/usr/bin/env python3\n" \
	"import plotly\n"

/* plot_header adds a header to the python plot file. */
static int plot_header(struct module *m)
{
	struct plot *this = (struct plot *)m->priv;

	return fprintf(this->f, PLOT_HEADER);
}

#define PLOT_FOOTER						      \
	"data = ["						      \
	"\tplotly.graph_objs.Scatter("				      \
	"\t\tx=time,"						      \
	"\t\ty=amplitude,"					      \
	"\t\tmode = 'lines',"					      \
	"\t),"							      \
	"]"							      \
	"layout = plotly.graph_objs.Layout("			      \
	"\ttitle='%s',"						      \
	"\txaxis=dict("						      \
	"\t\ttitle='%s',"					      \
	"\t),"							      \
	"\tyaxis=dict("						      \
	"\t\ttitle='%s',"					      \
	"\t\trangemode='tozero',"				      \
	"\t),"							      \
	")"							      \
	"figure = plotly.graph_objs.Figure(data=data, layout=layout)" \
	"plotly.offline.plot(figure, filename='%s.html')\n"

/* plot_footer adds a footer to the python plot file. */
static int plot_footer(struct module *m)
{
	struct plot *this = (struct plot *)m->priv;
	char name[128];

	snprintf(name, sizeof(name), "%s%d.html", this->cfg->name, this->idx);
	return fprintf(this->f, PLOT_FOOTER, this->cfg->title, this->cfg->x_name, this->cfg->y0_name, name);
}

/* plot_new_variable adds a new variable to the plot file. */
static int plot_new_variable(struct module *m, const char *name)
{
	struct plot *this = (struct plot *)m->priv;

	return fprintf(this->f, "%s = []\n", name);
}

/* plot_open opens a plot file. */
static int plot_open(struct module *m)
{
	struct plot *this = (struct plot *)m->priv;
	char name[128];

	snprintf(name, sizeof(name), "%s%d.py", this->cfg->name, this->idx);
	LOG_INF("open %s", name);
	FILE *f = fopen(name, "w");
	if (f == NULL) {
		LOG_ERR("unable to open plot file");
		return -1;
	}

	this->f = f;

	/* add header */
	int err = plot_header(m);
	if (err < 0) {
		LOG_ERR("unable to write plot header");
		fclose(this->f);
		return -1;
	}
	return 0;
}

/* plot_close closes the plot file. */
static void plot_close(struct module *m)
{
	struct plot *this = (struct plot *)m->priv;

	LOG_INF("close plot file");
	/* add footer */
	plot_footer(m);
	fclose(this->f);
	this->idx++;
}

/******************************************************************************
 * module port functions
 */

static void plot_port_trigger(struct module *m, const struct event *e)
{
	struct plot *this = (struct plot *)m->priv;
	bool trigger = event_get_bool(e);

	if (!trigger) {
		return;
	}

	if (this->triggered) {
		LOG_INF("%s_%08x already triggered", m->info->name, m->id);
		return;
	}

	// trigger!
	int err = plot_open(m);
	if (err != 0) {
		LOG_INF("can't open plot file (%d)", err);
		return;
	}

	plot_new_variable(m, this->cfg->x_name);
	plot_new_variable(m, this->cfg->y0_name);
	this->triggered = true;
	this->samples_left = this->samples;
}

/******************************************************************************
 * module functions
 */

static int plot_alloc(struct module *m, va_list vargs)
{
	/* allocate the private data */
	struct plot *this = ggm_calloc(1, sizeof(struct plot));

	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	return 0;
}

static void plot_free(struct module *m)
{
	struct plot *this = (struct plot *)m->priv;

	ggm_free(this);
}

static bool plot_process(struct module *m, float *bufs[])
{
	struct plot *this = (struct plot *)m->priv;
	float *out = bufs[0];

	(void)this;
	(void)out;

	return true;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "x", .type = PORT_TYPE_AUDIO },
	{ .name = "y0", .type = PORT_TYPE_AUDIO },
	{ .name = "trigger", .type = PORT_TYPE_BOOL, .func = plot_port_trigger },
	PORT_EOL,
};

const struct module_info view_plot_module = {
	.name = "view.plot",
	.in = in_ports,
	.alloc = plot_alloc,
	.free = plot_free,
	.process = plot_process,
};

MODULE_REGISTER(view_plot_module);

/*****************************************************************************/
