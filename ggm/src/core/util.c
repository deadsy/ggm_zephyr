/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Utility Functions
 */

#include "ggm.h"

/******************************************************************************
 * map_lin returns a linear mapping from x = 0..1 to y = y0..y1
 */

float map_lin(float x, float y0, float y1)
{
	return ((y1 - y0) * x) + y0;
}

/******************************************************************************
 * map_exp returns an exponential mapping from x = 0..1 to y = y0..y1
 * k < 0 and y1 > y0 gives y'' < 0 (downwards curve)
 * k > 0 and y1 > y0 gives y'' > 0 (upwards curve)
 * k != 0 and abs(k) is typically 3..5
 */

float map_exp(float x, float y0, float y1, float k)
{
	if (k == 0) {
		LOG_ERR("k == 0, use map_lin");
		return map_lin(x, y0, y1);
	}
	float a = (y0 - y1) / (1.f - pow2(k));
	float b = y0 - a;
	return (a * pow2(k * x)) + b;
}

/*****************************************************************************/
