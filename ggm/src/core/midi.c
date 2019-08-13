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

struct midi_msg_str {
	uint8_t msg;
	const char *str;
};

static const struct midi_msg_str midi_msg_string[] = {
	{ MIDI_STATUS_NOTEOFF, "note off" },
	{ MIDI_STATUS_NOTEON, "note on" },
	{ MIDI_STATUS_POLYPHONICAFTERTOUCH, "polyphonic aftertouch" },
	{ MIDI_STATUS_CONTROLCHANGE, "control change" },
	{ MIDI_STATUS_PROGRAMCHANGE, "program change" },
	{ MIDI_STATUS_CHANNELAFTERTOUCH, "channel aftertouch" },
	{ MIDI_STATUS_PITCHWHEEL, "pitch wheel" },
	{ MIDI_STATUS_SYSEXSTART, "sysex start" },
	{ MIDI_STATUS_QUARTERFRAME, "quarter frame" },
	{ MIDI_STATUS_SONGPOINTER, "song pointer" },
	{ MIDI_STATUS_SONGSELECT, "song select" },
	{ MIDI_STATUS_TUNEREQUEST, "tune request" },
	{ MIDI_STATUS_SYSEXEND, "sysex end" },
	{ MIDI_STATUS_TIMINGCLOCK, "timing clock" },
	{ MIDI_STATUS_START, "start" },
	{ MIDI_STATUS_CONTINUE, "continue" },
	{ MIDI_STATUS_STOP, "stop" },
	{ MIDI_STATUS_ACTIVESENSING, "active sensing" },
	{ MIDI_STATUS_RESET, "reset" },
	{ MIDI_STATUS_COMMON, "common" },
	{ MIDI_STATUS_REALTIME, "realtime" },
};

#define NUM_MIDI_MSG (sizeof(midi_msg_string) / sizeof(struct midi_msg_str))

char *midi_str(const struct event *e, char *s, size_t n)
{
	int i = 0;

	if (e->type != EVENT_TYPE_MIDI) {
		return NULL;
	}

	uint8_t msg = event_get_midi_msg(e);
	const char *msg_str = NULL;

	for (int j = 0; j < NUM_MIDI_MSG; j++) {
		if (midi_msg_string[j].msg == msg) {
			msg_str = midi_msg_string[j].str;
			break;
		}
	}

	if (msg_str != NULL) {
		i += snprintf(&s[i], n - i, "%s", msg_str);
	} else {
		i += snprintf(&s[i], n - i, "unknown(%02x)", msg);
	}

//	i += snprintf(&s[i], n - i, " status %02x arg0 %02x arg1 %02x", e->u.midi.status, e->u.midi.arg0, e->u.midi.arg1);
	return s;
}

/*****************************************************************************/
