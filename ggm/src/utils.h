/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_UTILS_H
#define GGM_SRC_UTILS_H

#include "const.h"

/******************************************************************************
 * Utility Functions
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

/* float2hex converts a 32-bit float to its uint32_t representation */
static inline uint32_t float2hex(float f)
{
	void *ptr = &f;

	return *(uint32_t *)ptr;
}

#endif /* GGM_SRC_UTILS_H */

