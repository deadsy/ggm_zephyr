/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

struct midi_mon {
	uint8_t chan; /* MIDI channel we are monitoring */
};

/******************************************************************************
 * module port functions
 */

static void midi_mon_port_midi(struct module *m, const struct event *e)
{
	struct midi_mon *this = (struct midi_mon *)m->priv;

	e =  event_filter_midi_channel(e, this->chan);
	if (e != NULL) {
		char tmp[32];
		LOG_INF("%s", log_strdup(midi_str(tmp, sizeof(tmp), e)));
	}
}

/******************************************************************************
 * module functions
 */

static int midi_mon_alloc(struct module *m, va_list vargs)
{
	LOG_MOD_NAME(m);

	/* allocate the private data */
	struct midi_mon *this = ggm_calloc(1, sizeof(struct midi_mon));
	if (this == NULL) {
		return -1;
	}
	m->priv = (void *)this;

	/* setup the channel to monitor */
	this->chan = va_arg(vargs, int);

	return 0;
}

static void midi_mon_free(struct module *m)
{
	struct midi_mon *this = (struct midi_mon *)m->priv;

	LOG_MOD_NAME(m);
	ggm_free(this);
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = midi_mon_port_midi },
	PORT_EOL,
};

const struct module_info midi_mon_module = {
	.name = "midi.mon",
	.in = in_ports,
	.alloc = midi_mon_alloc,
	.free = midi_mon_free,
};

MODULE_REGISTER(midi_mon_module);

/*****************************************************************************/
