/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_OSAL_H
#define GGM_SRC_INC_OSAL_H

/******************************************************************************
 * zephyr
 */

#if defined(__ZEPHYR__)

#include <zephyr.h>
#include <logging/log.h>

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
 * linux
 */

#elif defined(__LINUX__)

#include <time.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define CONFIG_BOARD "linux"

#define LOG_ERR printf
#define LOG_INF printf
#define LOG_WRN printf

static inline const char *log_strdup(const char *s)
{
	return s;
}

static inline int k_sleep(long ms)
{
	struct timespec req, rem;

	if (ms > 999) {
		req.tv_sec = (int)(ms / 1000);
		req.tv_nsec = (ms - ((long)req.tv_sec * 1000)) * 1000000;
	} else   {
		req.tv_sec = 0;
		req.tv_nsec = ms * 1000000;
	}

	return nanosleep(&req, &rem);
}

#define k_calloc calloc
#define k_free free

/*****************************************************************************/

#else
#error "define the OS type for this build"
#endif

/*****************************************************************************/

#endif /* GGM_SRC_INC_OSAL_H */

/*****************************************************************************/
