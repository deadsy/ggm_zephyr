/******************************************************************************
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * OS Abstraction Layer
 *
 */

#ifndef GGM_SRC_INC_OSAL_H
#define GGM_SRC_INC_OSAL_H

/*****************************************************************************/
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

static inline void ggm_mdelay(long ms)
{
	k_sleep(ms);
}

static inline void *ggm_calloc(size_t num, size_t size)
{
	return k_calloc(num, size);
}

static inline void ggm_free(void *ptr)
{
	k_free(ptr);
}

/*****************************************************************************/
#elif defined(__LINUX__)

#include <stdio.h>
#include <string.h>

#define CONFIG_BOARD "linux"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERR  1
#define LOG_LEVEL_WRN  2
#define LOG_LEVEL_INF  3
#define LOG_LEVEL_DBG  4

#define LOG_ERR(...) ggm_log(LOG_LEVEL_ERR, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_WRN(...) ggm_log(LOG_LEVEL_WRN, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_INF(...) ggm_log(LOG_LEVEL_INF, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)
#define LOG_DBG(...) ggm_log(LOG_LEVEL_DBG, __FILE__, __FUNCTION__, __LINE__, __VA_ARGS__)

static inline const char *log_strdup(const char *s)
{
	return s;
}

void ggm_mdelay(long ms);
void *ggm_calloc(size_t num, size_t size);
void ggm_free(void *ptr);
void ggm_log(int level, const char *filename, const char *funcname, int line, ...);

/*****************************************************************************/

#else
#error "define the OS type for this build"
#endif

/*****************************************************************************/

#endif /* GGM_SRC_INC_OSAL_H */

/*****************************************************************************/
