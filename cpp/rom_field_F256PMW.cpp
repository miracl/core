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

namespace F256PMW {

/* NUMS 256-bit modulus */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_28;

// Base Bits= 28
const BIG Modulus= {0xFFFFF43,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
const BIG ROI= {0xFFFFF42,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
const BIG R2modp= {0x0,0x8900000,0x8B,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0xBD;
#endif

#if CHUNK==64

using namespace B256_56;

// Base Bits= 56
const BIG Modulus= {0xFFFFFFFFFFFF43L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG ROI= {0xFFFFFFFFFFFF42L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
const BIG R2modp= {0x89000000000000L,0x8BL,0x0L,0x0L,0x0L};
const chunk MConst= 0xBDL;

#endif

}
