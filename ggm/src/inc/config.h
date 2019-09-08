/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Module Configuration
 */

#ifndef GGM_SRC_INC_CONFIG_H
#define GGM_SRC_INC_CONFIG_H

#ifndef GGM_SRC_INC_GGM_H
#warning "please include this file using ggm.h"
#endif

/******************************************************************************
 * MIDI control mapping
 * The MIDI map is a synth-level table that maps a MIDI channel/control-change
 * number onto a module:port path. A given ch/cc can map to mutiple
 * module ports, the MIDI message will be sent to all of them. The module:port
 * path can have wildcards (*,?) for cases where multiple sub-modules of the
 * same type have been created. E.g. polyphony.
 */

struct midi_cfg {
	const char *path;       /* module:port path */
	uint8_t ch;             /* MIDI channel */
	uint8_t cc;             /* MIDI control number */
};

#define MIDI_CFG_EOL { NULL, 0, 0 }

/******************************************************************************
 * Module Configuration
 */

struct module_cfg {
	const char *path;       /* path name to module or module:port */
	const void *cfg;        /* pointer to item config structure */
};

#define MODULE_CFG_EOL { NULL, NULL }

/******************************************************************************
 * Port Configuration
 */

/* port_float_cfg defines the configuration for a port with float events */
struct port_float_cfg {
	float initial;  /* initial value */
	int cc;         /* MIDI channel/cc config */
};

struct port_int_cfg {
	int initial;    /* initial value */
	int cc;         /* MIDI channel/cc config */
};

struct port_bool_cfg {
	bool initial;   /* initial value */
	int cc;         /* MIDI channel/cc config */
};

/* MIDI_CC encodes the MIDI channel and CC number as an integer.
 * A value of 0 means MIDI CC is not configured for the port.
 */
#define MIDI_CC(ch, cc) (((ch) << 16) | ((cc) << 8) | 255)

/*****************************************************************************/

#endif /* GGM_SRC_INC_CONFIG_H */

/*****************************************************************************/
