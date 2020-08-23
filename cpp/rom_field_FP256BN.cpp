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
#include "fp_FP256BN.h"

namespace FP256BN {

/* Curve FP512BN - Pairing friendly BN curve */

/* ISO curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

using namespace B256_28;

// Base Bits= 28

const BIG Modulus= {0xED33013,0x292DDBA,0x80A82D3,0x65FB129,0x49F0CDC,0x5EEE71A,0xD46E5F2,0xFFFCF0C,0xFFFFFFF,0xF};
const BIG ROI= {0xED33012,0x292DDBA,0x80A82D3,0x65FB129,0x49F0CDC,0x5EEE71A,0xD46E5F2,0xFFFCF0C,0xFFFFFFF,0xF};
const BIG R2modp= {0x3B9F8B,0xEDE3363,0xFEC54E8,0x92FFEE9,0x3C55F79,0x13C1C06,0xC0123FA,0xA12F2EA,0xE559B2A,0x8};
const chunk MConst= 0x537E5E5;
const BIG CRu= {0x3A1B807,0x1C0A24A,0x32D1EDB,0xD79DF19,0x8659BCD,0x4092101,0x13988E1,0x0,0x0,0x0};
const BIG Fra= {0xF943106,0x760328A,0xAB28F74,0x71511E3,0x7CF39A1,0x8DDB086,0x52D1A6E,0xCA786F3,0xD617662,0x3};
const BIG Frb= {0xF3EFF0D,0xB32AB2F,0xD57F35E,0xF4A9F45,0xCCFD33A,0xD113693,0x819CB83,0x3584819,0x29E899D,0xC};
const BIG SQRTm3= {0x78FC004,0xF119926,0x1B0451C,0xB6BF2F7,0x3D3D540,0xDDCA517,0xAD3D42F,0xFFFCF0C,0xFFFFFFF,0xF};

#endif

#if CHUNK==64

using namespace B256_56;

// Base Bits= 56
const BIG Modulus= {0x292DDBAED33013L,0x65FB12980A82D3L,0x5EEE71A49F0CDCL,0xFFFCF0CD46E5F2L,0xFFFFFFFFL};
const BIG ROI= {0x292DDBAED33012L,0x65FB12980A82D3L,0x5EEE71A49F0CDCL,0xFFFCF0CD46E5F2L,0xFFFFFFFFL};
const BIG R2modp= {0xEDE336303B9F8BL,0x92FFEE9FEC54E8L,0x13C1C063C55F79L,0xA12F2EAC0123FAL,0x8E559B2AL};
const chunk MConst= 0x6C964E0537E5E5L;
const BIG CRu= {0x1C0A24A3A1B807L,0xD79DF1932D1EDBL,0x40921018659BCDL,0x13988E1L,0x0L};
const BIG Fra= {0x760328AF943106L,0x71511E3AB28F74L,0x8DDB0867CF39A1L,0xCA786F352D1A6EL,0x3D617662L};
const BIG Frb= {0xB32AB2FF3EFF0DL,0xF4A9F45D57F35EL,0xD113693CCFD33AL,0x3584819819CB83L,0xC29E899DL};
const BIG SQRTm3= {0xF11992678FC004L,0xB6BF2F71B0451CL,0xDDCA5173D3D540L,0xFFFCF0CAD3D42FL,0xFFFFFFFFL};

#endif

}
