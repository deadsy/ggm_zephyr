/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "module.h"

/******************************************************************************
 * port_num_by_name returns the number of ports within a set matching a name.
 */

int port_num_by_name(const struct port_info port[], const char *name)
{
	int n = 0;
	int i = 0;

	while (port[i].type != PORT_TYPE_NULL) {
		if (strcmp(name, port[i].name) == 0) {
			n += 1;
		}
		i++;
	}

	return n;
}

/*****************************************************************************/
