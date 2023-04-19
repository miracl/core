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
#include "fp_F448.h"

/* Curve Ed448 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

// Base Bits= 29
const BIG_448_29 Modulus_F448= {0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FDFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFF};
const BIG_448_29 ROI_F448= {0x1FFFFFFE,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FDFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFF};
const BIG_448_29 R2modp_F448= {0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x3000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst_F448= 0x1;

#endif

#if CHUNK==64
// Base Bits= 58
const BIG_448_58 Modulus_F448= {0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
const BIG_448_58 ROI_F448= {0x3FFFFFFFFFFFFFEL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
const BIG_448_58 R2modp_F448= {0x200000000L,0x0L,0x0L,0x0L,0x3000000L,0x0L,0x0L,0x0L};
const chunk MConst_F448= 0x1L;


#endif


