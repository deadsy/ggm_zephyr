/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_MODULE_OSC_LFO_H
#define GGM_SRC_MODULE_OSC_LFO_H

/******************************************************************************
 * LFO wave shapes
 */

enum {
	LFO_TRIANGLE,           /* triangle */
	LFO_SAWDOWN,            /* falling sawtooth */
	LFO_SAWUP,              /* rising sawtooth */
	LFO_SQUARE,             /* square */
	LFO_SINE,               /* sine */
	LFO_SAMPLEANDHOLD,      /* random sample and hold */
	LFO_MAX                 /* must be last */
};

/*****************************************************************************/

#endif /* GGM_SRC_MODULE_OSC_LFO_H */

/*****************************************************************************/
