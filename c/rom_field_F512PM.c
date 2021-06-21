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
#include "fp_F512PM.h"

/* NUMS 512-bit modulus */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_512_29 Modulus_F512PM= {0x1FFFFDC7,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
const BIG_512_29 ROI_F512PM= {0x1FFFFDC6,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
const BIG_512_29 R2modp_F512PM= {0xB100000,0x278,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst_512PM= 0x239;
#endif

#if CHUNK==64
// Base Bits= 60
const BIG_512_60 Modulus_512PM= {0xFFFFFFFFFFFFDC7L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG_512_60 R2modp_512PM= {0x100000000000000L,0x4F0BL,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
const BIG_512_60 ROI_512PM= {0xFFFFFFFFFFFFDC6L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const chunk MConst_512PM= 0x239L;

#endif