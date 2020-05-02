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
#include "fp_SM2.h"

/* Curve SM2 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 28
const BIG_256_28 Modulus_SM2= {0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF};
const BIG_256_28 ROI_SM2= {0xFFFFFFE,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF};
const BIG_256_28 R2modp_SM2= {0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0};
const chunk MConst_SM2= 0x1;
#endif

#if CHUNK==64

// Base Bits= 56
const BIG_256_56 Modulus_SM2= {0xFFFFFFFFFFFFFFL,0xFFFF00000000FFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
const BIG_256_56 ROI_SM2= {0xFFFFFFFFFFFFFEL,0xFFFF00000000FFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
const BIG_256_56 R2modp_SM2= {0x3000000040000L,0xFFFFFFFE000000L,0x300000002L,0x10000000100L,0x60000L};
const chunk MConst_SM2= 0x1L;

#endif

