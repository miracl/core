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

namespace SECP160R1 {

/* Curve SECP160R1 */

#if CHUNK==16
using namespace B160_13;
// Base Bits= 13
const BIG Modulus= {0x1FFF,0x1FFF,0x1FDF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0xF};
const BIG R2modp= {0x0,0x20,0x0,0x800,0x0,0x0,0x4,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG ROI= {0x1FFE,0x1FFF,0x1FDF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0x1FFF,0xF};
const chunk MConst= 0x1;

#endif

#if CHUNK==32

using namespace B160_29;

// Base Bits= 29


const BIG Modulus= {0x1FFFFFFF,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF};
const BIG R2modp= {0x10000000,0x0,0x4,0x8,0x0,0x0};
const BIG ROI= {0x1FFFFFFE,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF};
const chunk MConst= 0x1;


#endif

#if CHUNK==64

using namespace B160_56;

// Base Bits= 56
const BIG Modulus= {0xFFFFFF7FFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
const BIG R2modp= {0x1000000010000L,0x400000L,0x0L};
const BIG ROI= {0xFFFFFF7FFFFFFEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
const chunk MConst= 0xFFFFFF80000001L;


#endif

}
