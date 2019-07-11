/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_UTIL_H
#define GGM_SRC_INC_UTIL_H

#include <inttypes.h>
#include "const.h"

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

/******************************************************************************
 * compile time string hashing, string length <= 32 characters
 */

#define SEED 5381

static inline unsigned int _SH(unsigned int e, unsigned char c)
{
	unsigned int _e = (unsigned int)e;

	return (_e << 5) + _e + (unsigned char)c;
}

#define _SH_1(a) _SH(SEED, (a)[0])
#define _SH_2(a) _SH(_SH_1(a), (a)[1])
#define _SH_3(a) _SH(_SH_2(a), (a)[2])
#define _SH_4(a) _SH(_SH_3(a), (a)[3])
#define _SH_5(a) _SH(_SH_4(a), (a)[4])
#define _SH_6(a) _SH(_SH_5(a), (a)[5])
#define _SH_7(a) _SH(_SH_6(a), (a)[6])
#define _SH_8(a) _SH(_SH_7(a), (a)[7])
#define _SH_9(a) _SH(_SH_8(a), (a)[8])
#define _SH_10(a) _SH(_SH_9(a), (a)[9])
#define _SH_11(a) _SH(_SH_10(a), (a)[10])
#define _SH_12(a) _SH(_SH_11(a), (a)[11])
#define _SH_13(a) _SH(_SH_12(a), (a)[12])
#define _SH_14(a) _SH(_SH_13(a), (a)[13])
#define _SH_15(a) _SH(_SH_14(a), (a)[14])
#define _SH_16(a) _SH(_SH_15(a), (a)[15])
#define _SH_17(a) _SH(_SH_16(a), (a)[16])
#define _SH_18(a) _SH(_SH_17(a), (a)[17])
#define _SH_19(a) _SH(_SH_18(a), (a)[18])
#define _SH_20(a) _SH(_SH_19(a), (a)[19])
#define _SH_21(a) _SH(_SH_20(a), (a)[20])
#define _SH_22(a) _SH(_SH_21(a), (a)[21])
#define _SH_23(a) _SH(_SH_22(a), (a)[22])
#define _SH_24(a) _SH(_SH_23(a), (a)[23])
#define _SH_25(a) _SH(_SH_24(a), (a)[24])
#define _SH_26(a) _SH(_SH_25(a), (a)[25])
#define _SH_27(a) _SH(_SH_26(a), (a)[26])
#define _SH_28(a) _SH(_SH_27(a), (a)[27])
#define _SH_29(a) _SH(_SH_28(a), (a)[28])
#define _SH_30(a) _SH(_SH_29(a), (a)[29])
#define _SH_31(a) _SH(_SH_30(a), (a)[30])
#define _SH_32(a) _SH(_SH_31(a), (a)[31])

/* initial check prevents too-large strings from compiling */
#define STRHASH(a) (						 \
		(void)(sizeof(int [(sizeof(a) > 33 ? -1 : 1)])), \
		(sizeof(a) == 1) ? SEED :			 \
		(sizeof(a) == 2) ? _SH_1(a) :			 \
		(sizeof(a) == 3) ? _SH_2(a) :			 \
		(sizeof(a) == 4) ? _SH_3(a) :			 \
		(sizeof(a) == 4) ? _SH_3(a) :			 \
		(sizeof(a) == 5) ? _SH_4(a) :			 \
		(sizeof(a) == 6) ? _SH_5(a) :			 \
		(sizeof(a) == 7) ? _SH_6(a) :			 \
		(sizeof(a) == 8) ? _SH_7(a) :			 \
		(sizeof(a) == 9) ? _SH_8(a) :			 \
		(sizeof(a) == 10) ? _SH_9(a) :			 \
		(sizeof(a) == 11) ? _SH_10(a) :			 \
		(sizeof(a) == 12) ? _SH_11(a) :			 \
		(sizeof(a) == 13) ? _SH_12(a) :			 \
		(sizeof(a) == 14) ? _SH_13(a) :			 \
		(sizeof(a) == 15) ? _SH_14(a) :			 \
		(sizeof(a) == 16) ? _SH_15(a) :			 \
		(sizeof(a) == 17) ? _SH_16(a) :			 \
		(sizeof(a) == 18) ? _SH_17(a) :			 \
		(sizeof(a) == 19) ? _SH_18(a) :			 \
		(sizeof(a) == 20) ? _SH_19(a) :			 \
		(sizeof(a) == 21) ? _SH_20(a) :			 \
		(sizeof(a) == 22) ? _SH_21(a) :			 \
		(sizeof(a) == 23) ? _SH_22(a) :			 \
		(sizeof(a) == 24) ? _SH_23(a) :			 \
		(sizeof(a) == 25) ? _SH_24(a) :			 \
		(sizeof(a) == 26) ? _SH_25(a) :			 \
		(sizeof(a) == 27) ? _SH_26(a) :			 \
		(sizeof(a) == 28) ? _SH_27(a) :			 \
		(sizeof(a) == 29) ? _SH_28(a) :			 \
		(sizeof(a) == 30) ? _SH_29(a) :			 \
		(sizeof(a) == 31) ? _SH_30(a) :			 \
		(sizeof(a) == 32) ? _SH_31(a) :			 \
		(sizeof(a) == 33) ? _SH_32(a) :			 \
		0)
/* last zero is unreachable */

#endif /* GGM_SRC_INC_UTIL_H */
