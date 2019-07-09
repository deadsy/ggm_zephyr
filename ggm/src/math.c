/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <string.h>

#include "ggm.h"

/******************************************************************************
 * 32-bit float helper macros
 */

/* A union which permits us to convert between a float and a 32 bit int.  */
typedef union {
	float value;
	uint32_t word;
} ieee_float_shape_type;

/* Get a 32 bit int from a float.  */
#define GET_FLOAT_WORD(i, d)		    \
	do {				    \
		ieee_float_shape_type gf_u; \
		gf_u.value = (d);	    \
		(i) = gf_u.word;	    \
	} while (0)

/* Set a float from a 32 bit int.  */
#define SET_FLOAT_WORD(d, i)		    \
	do {				    \
		ieee_float_shape_type sf_u; \
		sf_u.word = (i);	    \
		(d) = sf_u.value;	    \
	} while (0)

#define FLT_UWORD_IS_NAN(x) ((x) > 0x7f800000L)
#define FLT_UWORD_IS_INFINITE(x) ((x) == 0x7f800000L)

/******************************************************************************
 * 32-bit float truncation.
 */

float truncf(float x)
{
	int32_t signbit, w, exponent_less_127;

	GET_FLOAT_WORD(w, x);

	/* Extract sign bit. */
	signbit = w & 0x80000000;

	/* Extract exponent field. */
	exponent_less_127 = ((w & 0x7f800000) >> 23) - 127;

	if (exponent_less_127 < 23) {
		if (exponent_less_127 < 0) {
			/* -1 < x < 1, so result is +0 or -0. */
			SET_FLOAT_WORD(x, signbit);
		} else {
			SET_FLOAT_WORD(x, signbit | (w & ~(0x007fffff >> exponent_less_127)));
		}
	} else {
		if (exponent_less_127 == 255) {
			/* x is NaN or infinite. */
			return x + x;
		}

		/* All bits in the fraction field are relevant. */
	}
	return x;
}

/******************************************************************************
 * 32-bit float absolute values
 */

float fabsf(float x)
{
	union {
		unsigned int ui;
		float f;
	} val;

	val.f = x;
	val.ui = (val.ui << 1) >> 1;
	return val.f;
}

/******************************************************************************
 * is not-a-number
 */

int isnanf(float x)
{
	int32_t ix;

	GET_FLOAT_WORD(ix, x);
	ix &= 0x7fffffff;
	return FLT_UWORD_IS_NAN(ix);
}

/******************************************************************************
 * is infinite
 */

int isinff(float x)
{
	int32_t ix;

	GET_FLOAT_WORD(ix, x);
	ix &= 0x7fffffff;
	return FLT_UWORD_IS_INFINITE(ix);
}

/******************************************************************************
 * 32-bit float trigonometry functions
 * These are LUT based and are less accurate but faster than typical libm functions.
 */

float cosf(float x)
{
	uint32_t xi = (uint32_t) (fabsf(x) * PhaseScale);

	return cos_lookup(xi);
}

float sinf(float x)
{
	uint32_t xi = QuarterCycle - (uint32_t) (fabsf(x) * PhaseScale);

	return cos_lookup(xi);
}

float tanf(float x)
{
	return sinf(x) / cosf(x);
}

/******************************************************************************
 * 32-bit float power function
 * powe returns powf(e, x)
 */

#define LOG_E2 (1.4426950408889634f)    /* 1.0 / math.log(2.0) */

float powe(float x)
{
	return pow2(LOG_E2 * x);
}

/******************************************************************************
 * float to string conversion
 * https://blog.benoitblanchon.fr/lightweight-float-to-string/
 */

static void split_float(float val, uint32_t *whole, uint32_t *frac, int16_t *exp)
{
	*whole = 0;
	*frac = 0;
	*exp = 0;
}

/* str2str copies a string into a buffer */
static int str2str(char *str, char *buf)
{
	int i = 0;

	while (str[i] != 0) {
		buf[i] = str[i];
		i++;
	}
	return i - 1;
}

/* int2str creates a decimal number string in a buffer */
static int int2str(uint32_t val, char *buf)
{
	int i = 0;

	/*work out the decimal string*/
	do {
		buf[i++] = (val % 10) + '0';
		val /= 10;
	} while (val);
	buf[i] = 0;
	int n = i;
	i -= 1;
	/*reverse the string*/
	for (int j = 0; j < i; j++, i--) {
		char tmp = buf[j];
		buf[j] = buf[i];
		buf[i] = tmp;
	}
	return n;
}

static int frac2str(uint32_t val, char *buf)
{
	return 0;
}

char *float2str(float val, char *buf)
{
	int i = 0;

	if (isnanf(val)) {
		i += str2str("nan", &buf[i]);
		return buf;
	}

	if (val < 0.f) {
		i += str2str("-", &buf[i]);
		val = -val;
	}

	if (isinff(val)) {
		i += str2str("inf", &buf[i]);
		return buf;
	}

	uint32_t whole, frac;
	int16_t exp;
	split_float(val, &whole, &frac, &exp);

	i += int2str(whole, &buf[i]);

	if (frac) {
		i += frac2str(frac, &buf[i]);
	}

	if (exp < 0) {
		i += str2str("e-", &buf[i]);
		i += int2str(-exp, &buf[i]);
	}

	if (exp > 0) {
		i += str2str("e", &buf[i]);
		i += int2str(exp, &buf[i]);
	}

	return buf;
}
