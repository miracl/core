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
#include "fp_NIST521.h"

namespace NIST521 {

/* Curve NIST521 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B528_28;

// Base Bits= 28
const BIG Modulus= {0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0x1FFFF};
const BIG ROI= {0xFFFFFFE,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0x1FFFF};
const BIG R2modp= {0x400000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0x1;
#endif

#if CHUNK==64

using namespace B528_60;

// Base Bits= 60
const BIG Modulus= {0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x1FFFFFFFFFFL};
const BIG ROI= {0xFFFFFFFFFFFFFFEL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x1FFFFFFFFFFL};
const BIG R2modp= {0x4000000000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
const chunk MConst= 0x1L;
#endif

}
