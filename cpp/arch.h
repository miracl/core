/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Architecture definition header file */

/**
 * @file arch.h
 * @author Mike Scott
 * @date 23rd February 2016
 * @brief Architecture Header File
 *
 * Specify Processor Architecture
 *
 */

/* NOTE: There is only one user configurable section in this header - see below */

#ifndef ARCH_H
#define ARCH_H
#include <stdint.h>

#define WOULD_USE_VLAS   /* Would like to use variable length arrays? */

#ifdef WOULD_USE_VLAS
#if __cplusplus >= 201402L
#define USE_VLAS
#endif
#endif

namespace core {

/*** START OF USER CONFIGURABLE SECTION - set architecture ***/

#define CHUNK @WL@      /**< size of chunk in bits = wordlength of computer = 16, 32 or 64. Note not all curve options are supported on 16-bit processors */

/*** END OF USER CONFIGURABLE SECTION ***/

/* Create Integer types */

//#define byte uint8_t            /**< 8-bit unsigned integer */
//#define sign8 int8_t            /**< 8-bit signed integer */
//#define sign16 int16_t          /**< 16-bit signed integer */
//#define sign32 int32_t          /**< 32-bit signed integer */
//#define sign64 int64_t          /**< 64-bit signed integer */
//#define unsign32 uint32_t       /**< 32-bit unsigned integer */
//#define unsign64 uint64_t       /**< 64-bit unsigned integer */

using byte = uint8_t;			/**< 8-bit unsigned integer */
using sign8 = int8_t;			/**< 8-bit signed integer */
using sign16 = int16_t;			/**< 16-bit signed integer */
using sign32 = int32_t;			/**< 32-bit signed integer */
using sign64 = int64_t;			/**< 64-bit signed integer */
using unsign32 = uint32_t;		/**< 32-bit unsigned integer */
using unsign64 = uint64_t;		/**< 64-bit unsigned integer */

//#define uchar unsigned char  /**<  Unsigned char */
using uchar = unsigned char;

/* Don't mess with anything below this line unless you know what you are doing */

#if CHUNK==16

#define chunk int16_t       /**< C type corresponding to word length */
#define dchunk int32_t      /**< Always define double length chunk type if available */

#endif

#if CHUNK == 32

#define chunk int32_t       /**< C type corresponding to word length */
#define dchunk int64_t      /**< Always define double length chunk type if available */

#endif

#if CHUNK == 64

#define chunk int64_t       /**< C type corresponding to word length */
#if defined(__SIZEOF_INT128__) && __SIZEOF_INT128__ == 16
#define dchunk __int128
#endif

#endif

#ifdef dchunk
#define COMBA      /**< Use COMBA method for faster muls, sqrs and reductions */
#endif

}

#endif
