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
#include "fp_F256PMW.h"

/* NUMS 256-bit modulus */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 28
const BIG_256_28 Modulus_F256PMW= {0xFFFFF43,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
const BIG_256_28 ROI_F256PMW= {0xFFFFF42,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
const BIG_256_28 R2modp_F256PMW= {0x0,0x8900000,0x8B,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst_F256PMW= 0xBD;
#endif

#if CHUNK==64
// Base Bits= 56
const BIG_256_56 Modulus_F256PMW= {0xFFFFFFFFFFFF43L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG_256_56 ROI_F256PMW= {0xFFFFFFFFFFFF42L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG_256_56 R2modp_F256PMW= {0x89000000000000L,0x8BL,0x0L,0x0L,0x0L};
const chunk MConst_F256PMW= 0xBDL;

#endif