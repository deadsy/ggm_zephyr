/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <stdio.h>

#include "ggm.h"

/******************************************************************************
 * midi_to_frequency converts a MIDI note to a frequency value (Hz).
 * The note value is a float for pitch bending, tuning, etc.
 */

float midi_to_frequency(float note)
{
	return 440.f * pow2((note - 69.f) * (1.f / 12.f));
}

/******************************************************************************
 * midi_pitch_bend maps a pitch bend value onto a MIDI note offset.
 */
float midi_pitch_bend(uint16_t val)
{
	/* 0..8192..16383 maps to -/+ 2 semitones */
	return ((float)val - 8192.f) * (2.f / 8192.f);
}

/******************************************************************************
 * midi_str returns a descriptive string for a MIDI event
 */

static const char *midi_msg_channel[] = {
	"?(00)",                        /* 0x00 */
	"?(10)",                        /* 0x10 */
	"?(20)",                        /* 0x20 */
	"?(30)",                        /* 0x30 */
	"?(40)",                        /* 0x40 */
	"?(50)",                        /* 0x50 */
	"?(60)",                        /* 0x60 */
	"?(70)",                        /* 0x70 */
	"note off",                     /* 0x80 */
	"note on",                      /* 0x90 */
	"polyphonic aftertouch",        /* 0xa0 */
	"control change",               /* 0xb0 */
	"program change",               /* 0xc0 */
	"channel aftertouch",           /* 0xd0 */
	"pitch wheel",                  /* 0xe0 */
	NULL,                           /* 0xf0 */
};

static const char *midi_msg_system[] = {
	"sysex start",          /* 0xf0 */
	"quarter frame",        /* 0xf1 */
	"song pointer",         /* 0xf2 */
	"song select",          /* 0xf3 */
	"?(f4)",                /* 0xf4 */
	"?(f5)",                /* 0xf5 */
	"tune request",         /* 0xf6 */
	"sysex end",            /* 0xf7 */
	"timing clock",         /* 0xf8 */
	"?(f9)",                /* 0xf9 */
	"start",                /* 0xfa */
	"continue",             /* 0xfb */
	"stop",                 /* 0xfc */
	"?(fd)",                /* 0xfd */
	"active sensing",       /* 0xfe */
	"reset",                /* 0xff */
};

char *midi_str(const struct event *e, char *s, size_t n)
{
	int i = 0;

	if (e->type != EVENT_TYPE_MIDI) {
		return NULL;
	}

	uint8_t status = e->u.midi.status;
	const char *msg;
	if ((status & 0xf0) == 0xf0) {
		msg = midi_msg_system[status & 15];
	} else {
		msg = midi_msg_channel[status >> 4];
	}

	i += snprintf(&s[i], n - i, "%s", msg);

//	i += snprintf(&s[i], n - i, " status %02x arg0 %02x arg1 %02x", e->u.midi.status, e->u.midi.arg0, e->u.midi.arg1);
	return s;
}

/*****************************************************************************/
