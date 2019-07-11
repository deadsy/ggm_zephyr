/*
 * Copyright (c) 2019 Jason T. Harris. (sirmanlypowers@gmail.com)
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef GGM_SRC_INC_PROTO_H
#define GGM_SRC_INC_PROTO_H

#include <stddef.h>
#include <inttypes.h>

/******************************************************************************
 * Lookup Table Functions
 */

float cos_lookup(uint32_t x);
float pow2(float x);

/******************************************************************************
 * 32-bit float math functions.
 */

float truncf(float x);
float fabsf(float x);

int isinff(float x);
int isnanf(float x);

float cosf(float x);
float sinf(float x);
float tanf(float x);

float powe(float x);

/******************************************************************************
 * 32-bit float to string conversion
 */

char *ftoa(float val, char *buf);
int float2str(float val, char *buf);

/******************************************************************************
 * block operations
 */

void block_mul(float *out, float *buf, size_t n);
void block_mul_k(float *out, float k, size_t n);
void block_add(float *out, float *buf, size_t n);
void block_add_k(float *out, float k, size_t n);
void block_copy(float *dst, const float *src, size_t n);
void block_copy_mul_k(float *dst, const float *src, float k, size_t n);

#endif /* GGM_SRC_INC_PROTO_H */
