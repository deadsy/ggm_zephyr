/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_UTIL_H
#define GGM_SRC_INC_UTIL_H

/******************************************************************************
 * radian/degree conversion
 */

/* d2r converts degrees to radians */
static inline float d2r(float d)
{
	return d * (Pi / 180.f);
}

/* r2d converts radians to degrees */
static inline float r2d(float r)
{
	return r * (180.f / Pi);
}

/******************************************************************************
 * clamp floating point values
 */

/* clamp x between a and b */
static inline float clampf(float x, float a, float b)
{
	if (x < a) {
		return a;
	}
	if (x > b) {
		return b;
	}
	return x;
}

/* clamp x to >= a */
static inline float clampf_lo(float x, float a)
{
	return (x < a) ? a : x;
}

/* clamp x to <= a */
static inline float clampf_hi(float x, float a)
{
	return (x > a) ? a : x;
}

/******************************************************************************
 * linear mapping map x = 0..1 to y = a..b
 */

static inline float mapf(float x, float a, float b)
{
	return ((b - a) * x) + a;
}

/******************************************************************************
 * Simple random number generation
 * Based on a linear congruential generator.
 */

/* set an initial value for the random state */
static inline void rand_init(uint32_t seed, uint32_t *state)
{
	if (seed == 0) {
		seed = 1;
	}
	*state = seed;
}

/* return a random uint32_t (0..0x7fffffff) */
static inline uint32_t rand_uint32(uint32_t *state)
{
	*state = ((*state * 1103515245) + 12345) & 0x7fffffff;
	return *state;
}

/* return a float from -1..1 */
static inline float randf(uint32_t *state)
{
	union {
		uint32_t ui;
		float f;
	} val;

	val.ui = (rand_uint32(state) & 0x007fffff) | (128 << 23);       /* 2..4 */
	return val.f - 3.f;                                             /* -1..1 */
}

/******************************************************************************
 * float to uint32_t conversion
 */

static inline uint32_t float2uint(float x)
{
	union {
		uint32_t ui;
		float f;
	} val;

	val.f = x;
	return val.ui;
}

/*****************************************************************************/

#endif /* GGM_SRC_INC_UTIL_H */

/*****************************************************************************/
