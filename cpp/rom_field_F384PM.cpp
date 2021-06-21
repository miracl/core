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
#include "fp_F384PM.h"

namespace F384PM {

/* NUMS 384-bit modulus */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B384_29;

// Base Bits= 29
const BIG Modulus= {0x1FFFFEC3,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
const BIG ROI= {0x1FFFFEC2,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
const BIG R2modp= {0x0,0x4448000,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst= 0x13D;
#endif

#if CHUNK==64

using namespace B384_58;

// Base Bits= 58
const BIG Modulus= {0x3FFFFFFFFFFFEC3L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const BIG R2modp= {0x88900000000000L,0x6L,0x0L,0x0L,0x0L,0x0L,0x0L};
const BIG ROI= {0x3FFFFFFFFFFFEC2L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const chunk MConst= 0x13DL;
#endif

}
