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
#include "arch.h"
#include "fp_F384PM.h"

/* NUMS 384-bit modulus */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_384_29 Modulus_F384PM= {0x1FFFFEC3,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
const BIG_384_29 ROI_F384PM= {0x1FFFFEC2,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
const BIG_384_29 R2modp_F384PM= {0x0,0x4448000,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst_F384PM= 0x13D;
#endif

#if CHUNK==64
// Base Bits= 58
const BIG_384_58 Modulus_384PM= {0x3FFFFFFFFFFFEC3L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const BIG_384_58 R2modp_384PM= {0x88900000000000L,0x6L,0x0L,0x0L,0x0L,0x0L,0x0L};
const BIG_384_58 ROI_384PM= {0x3FFFFFFFFFFFEC2L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const chunk MConst_384PM= 0x13DL;
#endif