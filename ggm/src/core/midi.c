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
 * midi_str returns a descriptive string for the MIDI event
 */

char *midi_str(const struct event *e, char *s, size_t n)
{
	int i = 0;

	if (e->type != EVENT_TYPE_MIDI) {
		return NULL;
	}

	switch (e->u.midi.type) {

	case MIDI_EVENT_NULL:
		i += snprintf(&s[i], n - i, "null");
		break;
	case MIDI_EVENT_NOTE_ON:
		i += snprintf(&s[i], n - i, "note_on");
		break;
	case MIDI_EVENT_NOTE_OFF:
		i += snprintf(&s[i], n - i, "note_off");
		break;
	case  MIDI_EVENT_CONTROL_CHANGE:
		i += snprintf(&s[i], n - i, "control_change");
		break;
	case MIDI_EVENT_PITCH_WHEEL:
		i += snprintf(&s[i], n - i, "pitch_wheel");
		break;
	case  MIDI_EVENT_POLYPHONIC_AFTERTOUCH:
		i += snprintf(&s[i], n - i, "polyphonic_aftertouch");
		break;
	case MIDI_EVENT_PROGRAM_CHANGE:
		i += snprintf(&s[i], n - i, "program_change");
		break;
	case MIDI_EVENT_CHANNEL_AFTERTOUCH:
		i += snprintf(&s[i], n - i, "channel_aftertouch");
		break;
	default:
		i += snprintf(&s[i], n - i, "unknown (0x%02x)", e->u.midi.type);
		return s;
	}

	i += snprintf(&s[i], n - i, " status %02x arg0 %02x arg1 %02x", e->u.midi.status, e->u.midi.arg0, e->u.midi.arg1);
	return s;
}

/*****************************************************************************/
