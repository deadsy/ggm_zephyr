/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Polyphonic Module
 * Manage concurrent instances (voices) of a given sub-module.
 * Note: The single channel output is the sum of outputs from each single channel voice.
 */

#include "ggm.h"

/******************************************************************************
 * private state
 */

#define MAX_POLYPHONY 8

struct voice {
	struct module *m;       /* the voice module */
	uint8_t note;           /* the MIDI note for this voice */
};

struct poly {
	uint8_t chan;                           /* MIDI channel we are using */
	struct voice voice[MAX_POLYPHONY];      /* voices*/
	int idx;                                /* round robin voice index */
	float bend;                             /* pitch bend value for all voices */
};

/******************************************************************************
 * voice functions
 */

/* voice_lookup returns the voice module for this MIDI note (or NULL) */
static struct module *voice_lookup(struct module *m, uint8_t note)
{
	struct poly *this = (struct poly *)m->priv;

	for (int i = 0; i < MAX_POLYPHONY; i++) {
		if (this->voice[i].note == note) {
			return this->voice[i].m;
		}
	}
	return NULL;
}

/* voice_alloc allocates a new voice module for the MIDI note */
static struct module *voice_alloc(struct module *m, uint8_t note)
{
	struct poly *this = (struct poly *)m->priv;

	LOG_INF("note %d", note);

	/* do round-robin voice allocation */
	struct voice *v = &this->voice[this->idx];
	this->idx += 1;
	if (this->idx == MAX_POLYPHONY) {
		this->idx = 0;
	}

	/* quiet the existing voice */
	event_in_float(v->m, "gate", -1.f, NULL);

	/* set the voice note */
	event_in_float(v->m, "note", (float)note + this->bend, NULL);
	v->note = note;

	return v->m;
}

/******************************************************************************
 * module port functions
 */

static void poly_port_midi(struct module *m, const struct event *e)
{
	struct poly *this = (struct poly *)m->priv;

	e =  event_filter_midi_channel(e, this->chan);
	if (e == NULL) {
		/* it's not for this channel */
		return;
	}

	switch (event_get_midi_msg(e)) {
	case MIDI_STATUS_NOTEON: {
		uint8_t note = event_get_midi_note(e);
		float vel = event_get_midi_velocity_float(e);
		struct module *v = voice_lookup(m, note);
		if (v == NULL) {
			v = voice_alloc(m, note);
		}
		/* note: vel = 0 is the same as note off (gate=0) */
		event_in_float(v, "gate", vel, NULL);
		break;
	}
	case MIDI_STATUS_NOTEOFF: {
		struct module *v = voice_lookup(m, event_get_midi_note(e));
		if (v != NULL) {
			/* send a note off control event, ignore the note off velocity (for now) */
			event_in_float(v, "gate", 0.f, NULL);
		}
		break;
	}
	case MIDI_STATUS_PITCHWHEEL: {
		/* get the pitch bend value */
		this->bend = midi_pitch_bend(event_get_midi_pitch_wheel(e));
		/* update all voices */
		for (int i = 0; i < MAX_POLYPHONY; i++) {
			struct voice *v = &this->voice[i];
			event_in_float(v->m, "note", (float)(v->note) + this->bend, NULL);
		}
		break;
	}
	default: {
		/* pass through the MIDI event to the voices */
		for (int i = 0; i < MAX_POLYPHONY; i++) {
			event_in(this->voice[i].m, "midi", e, NULL);
		}
		break;
	}
	}

}

/******************************************************************************
 * module functions
 */

static int poly_alloc(struct module *m, va_list vargs)
{
	LOG_MOD_NAME(m);

	/* allocate the private data */
	struct poly *this = ggm_calloc(1, sizeof(struct poly));
	if (this == NULL) {
		LOG_ERR("could not allocate private data");
		return -1;
	}
	m->priv = (void *)this;

	/* allocate the voices */
	module_func new_voice = va_arg(vargs, module_func);
	for (int i = 0; i < MAX_POLYPHONY; i++) {
		this->voice[i].m = new_voice(m->top);
		if (this->voice[i].m == NULL) {
			LOG_ERR("could not create voice module");
			goto error;
		}
	}

	return 0;

error:
	for (int i = 0; i < MAX_POLYPHONY; i++) {
		module_del(this->voice[i].m);
	}
	ggm_free(this);
	return -1;
}

static void poly_free(struct module *m)
{
	struct poly *this = (struct poly *)m->priv;

	LOG_MOD_NAME(m);
	for (int i = 0; i < MAX_POLYPHONY; i++) {
		module_del(this->voice[i].m);
	}
	ggm_free(this);
}

static bool poly_process(struct module *m, float *bufs[])
{
	struct poly *this = (struct poly *)m->priv;
	float *out = bufs[0];
	bool active = false;

	// zero the output buffer
	block_zero(out);

	// run each voice
	for (int i = 0; i < MAX_POLYPHONY; i++) {
		struct module *v = this->voice[i].m;
		float vbuf[AudioBufferSize];
		float *vbufs[] = { vbuf, };

		if (v->info->process(v, vbufs)) {
			block_add(out, vbuf);
			active = true;
		}
	}

	return active;
}

/******************************************************************************
 * module information
 */

static const struct port_info in_ports[] = {
	{ .name = "midi", .type = PORT_TYPE_MIDI, .func = poly_port_midi },
	PORT_EOL,
};

static const struct port_info out_ports[] = {
	{ .name = "out", .type = PORT_TYPE_AUDIO, },
	PORT_EOL,
};

const struct module_info midi_poly_module = {
	.name = "midi.poly",
	.in = in_ports,
	.out = out_ports,
	.alloc = poly_alloc,
	.free = poly_free,
	.process = poly_process,
};

MODULE_REGISTER(midi_poly_module);

/*****************************************************************************/
