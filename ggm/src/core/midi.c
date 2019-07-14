/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

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

/*****************************************************************************/
