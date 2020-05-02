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
#include "fp_C1665.h"

/* Curve C1665 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_168_29 Modulus_C1665= {0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF};
const BIG_168_29 R2modp_C1665= {0x190000,0x0,0x0,0x0,0x0,0x0};
const BIG_168_29 ROI_C1665= {0x1FFFFFFA,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF};
const chunk MConst_C1665= 0x5;

#endif

#if CHUNK==64
// Base Bits= 60
const BIG_168_60 Modulus_C1665= {0xFFFFFFFFFFFFFFBL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
const BIG_168_60 R2modp_C1665= {0x190000000L,0x0L,0x0L};
const BIG_168_60 ROI_C1665= {0xFFFFFFFFFFFFFFAL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
const chunk MConst_C1665= 0x5L;
#endif

