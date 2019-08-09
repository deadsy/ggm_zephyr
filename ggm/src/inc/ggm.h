/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_GGM_H
#define GGM_SRC_INC_GGM_H

#include <stddef.h>
#include <inttypes.h>

#include <logging/log.h>

/******************************************************************************
 * logging
 */

/* Display all messages */
#define LOG_LEVEL LOG_LEVEL_DBG

/* Set the module name for log messages */
#define LOG_MODULE_NAME ggm

#ifdef GGM_MAIN
LOG_MODULE_REGISTER(LOG_MODULE_NAME);
#else
LOG_MODULE_DECLARE(LOG_MODULE_NAME);
#endif

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

/******************************************************************************
 * top-level synth structure
 */

struct synth {
	struct module *root; /* root patch */
};

/******************************************************************************
 * Lookup Table Functions
 */

float cos_lookup(uint32_t x);
float pow2(float x);

/******************************************************************************
 * 32-bit float math functions.
 */

float truncf(float x);
float fabsf(float x);

int isinff(float x);
int isnanf(float x);

float cosf(float x);
float sinf(float x);
float tanf(float x);

float powe(float x);

/******************************************************************************
 * 32-bit float to string conversion
 */

char *ftoa(float val, char *buf);
int float2str(float val, char *buf);

/******************************************************************************
 * MIDI
 */

float midi_to_frequency(float note);
float midi_pitch_bend(uint16_t val);

/******************************************************************************
 * block operations
 */

void block_mul(float *out, float *buf, size_t n);
void block_mul_k(float *out, float k, size_t n);
void block_add(float *out, float *buf, size_t n);
void block_add_k(float *out, float k, size_t n);
void block_copy(float *dst, const float *src, size_t n);
void block_copy_mul_k(float *dst, const float *src, float k, size_t n);

/*****************************************************************************/

#endif /* GGM_SRC_INC_GGM_H */

/*****************************************************************************/
