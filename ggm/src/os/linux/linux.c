/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * OS Abstraction Layer for Linux
 */

#include <time.h>
#include <stdlib.h>

#include "ggm.h"

/******************************************************************************
 * ggm_mdelay delays for a specified number of milliseconds
 */

void ggm_mdelay(long ms)
{
	struct timespec req, rem;

	if (ms > 999) {
		req.tv_sec = (int)(ms / 1000);
		req.tv_nsec = (ms - ((long)req.tv_sec * 1000)) * 1000000;
	} else {
		req.tv_sec = 0;
		req.tv_nsec = ms * 1000000;
	}

	nanosleep(&req, &rem);
}

/******************************************************************************
 * memory allocation.
 */

void *ggm_calloc(size_t num, size_t size)
{
	return calloc(num, size);
}

void ggm_free(void *ptr)
{
	return free(ptr);
}

/******************************************************************************
 * logging
 */

static const char *level_str[] = {
	"none",
	"err",
	"wrn",
	"inf",
	"dbg",
};

static const char *strip_prefix(const char *prefix, const char *s)
{
	int n = strlen(prefix);

	if (strncmp(prefix, s, n) == 0) {
		s = &s[n];
	}
	return s;
}

void ggm_log(int level, const char *filename, const char *funcname, int line, ...)
{
	va_list args;

	/* header */
	char hdr[128];

	filename = strip_prefix("ggm/src/", filename);
	snprintf(hdr, sizeof(hdr), "%s %s:%s(%d)", level_str[level], filename, funcname, line);

	/* message */
	char msg[128];
	va_start(args, line);
	const char *fmt = va_arg(args, char *);
	vsnprintf(msg, sizeof(msg), fmt, args);
	va_end(args);

	printf("%-60s %s\n", hdr, msg);
}

/*****************************************************************************/
