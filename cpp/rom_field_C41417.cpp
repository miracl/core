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
#include "fp_C41417.h"

namespace C41417 {

/* Curve C41417 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B416_29;

// Base Bits= 29
const BIG Modulus= {0x1FFFFFEF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFF};
const BIG ROI= {0x1FFFFFEE,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFF};
const BIG R2modp= {0x0,0x242000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0x11;
#endif

#if CHUNK==64

using namespace B416_60;

// Base Bits= 60
const BIG Modulus= {0xFFFFFFFFFFFFFEFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFL};
const BIG ROI= {0xFFFFFFFFFFFFFEEL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFL};
const BIG R2modp= {0x121000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
const chunk MConst= 0x11L;
#endif

}
