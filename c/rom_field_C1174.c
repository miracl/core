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
#include "fp_C1174.h"

/* Curve C1174 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_256_29 Modulus_C1174= {0x1FFFFFF7,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
const BIG_256_29 R2modp_C1174= {0x5100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 ROI_C1174= {0x1FFFFFF6,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
const chunk MConst_C1174= 0x9;

#endif

#if CHUNK==64
// Base Bits= 56
const BIG_256_56 Modulus_C1174= {0xFFFFFFFFFFFFF7L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
const BIG_256_56 ROI_C1174= {0xFFFFFFFFFFFFF6L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
const BIG_256_56 R2modp_C1174= {0x0L,0x144L,0x0L,0x0L,0x0L};
const chunk MConst_C1174= 0x9L;
#endif
