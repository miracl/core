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
#include "fp_SECP160R1.h"

/* Curve SECP160R1 */

#if CHUNK==16

// Base Bits= 13
const BIG_160_13 Modulus_SECP160R1= {0x1FFF,0x1FFF,0x1FDF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0xF};
const BIG_160_13 R2modp_SECP160R1= {0x0,0x20,0x0,0x800,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_160_13 ROI_SECP160R1= {0x1FFE,0x1FFF,0x1FDF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0xF};
const chunk MConst_SECP160R1= 0x1;

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_160_29 Modulus_SECP160R1= {0x1FFFFFFF,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF};
const BIG_160_29 R2modp_SECP160R1= {0x10000000,0x0,0x4,0x8,0x0,0x0};
const BIG_160_29 ROI_SECP160R1= {0x1FFFFFFE,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF};
const chunk MConst_SECP160R1= 0x1;


#endif

#if CHUNK==64

// Base Bits= 56
const BIG_160_56 Modulus_SECP160R1= {0xFFFFFF7FFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
const BIG_160_56 R2modp_SECP160R1= {0x1000000010000L,0x400000L,0x0L};
const BIG_160_56 ROI_SECP160R1= {0xFFFFFF7FFFFFFEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
const chunk MConst_SECP160R1= 0xFFFFFF80000001L;


#endif

