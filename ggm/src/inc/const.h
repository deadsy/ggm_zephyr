/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_CONST_H
#define GGM_SRC_INC_CONST_H

/******************************************************************************
 * Constants
 */

/* Pi (3.14159...) */
#define Pi (3.1415926535897932384626433832795f)

/* Tau (2 * Pi) */
#define Tau (2.f * Pi)

/* AudioSampleFrequency is the sample frequency for audio (Hz) */
#define AudioSampleFrequency (48000U)

/* AudioSamplePeriod is the sample period for audio (seconds) */
#define AudioSamplePeriod (1.f / (float)AudioSampleFrequency)

/* AudioBufferSize is the number of float samples per audio buffer */
#define AudioBufferSize (128U)

/* SecsPerAudioBuffer is the audio duration for a single audio buffer */
#define SecsPerAudioBuffer ((float)AudioBufferSize / (float)AudioSampleFrequency)

/* FullCycle is a full uint32_t phase count */
#define FullCycle (1ULL << 32)

/* HalfCycle is a half uint32_t phase count */
#define HalfCycle (1U << 31)

/* QuarterCycle is a quarter uint32_t phase count */
#define QuarterCycle (1U << 30)

/* FrequencyScale scales a frequency value to a uint32_t phase step value */
#define FrequencyScale ((float)FullCycle / (float)AudioSampleFrequency)

/* PhaseScale scales a phase value to a uint32_t phase step value */
#define PhaseScale ((float)FullCycle / Tau)

#endif /* GGM_SRC_INC_CONST_H */
