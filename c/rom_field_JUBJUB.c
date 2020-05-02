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
#include "fp_JUBJUB.h"

/* JUBJUB Modulus  */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_256_29 Modulus_JUBJUB= {0x1,0x1FFFFFF8,0x1F96FFBF,0x1B4805FF,0x1D80553B,0xC0404D0,0x1520CCE7,0xA6533AF,0x73EDA7};
const BIG_256_29 ROI_JUBJUB= {0xB912F1F,0x1BC47A80,0xCF82506,0x49FE4E1,0xD6C0C40,0x16E468B4,0xDBC3F55,0x13CB682D,0x212D7};
const BIG_256_29 R2modp_JUBJUB= {0xA71B3C0,0x1D32207E,0x1663D999,0x1C5ABC93,0x3B58C44,0xBE37438,0x829F771,0x1660139E,0x27FD91};
const chunk MConst_JUBJUB= 0x1FFFFFFF;
#endif

#if CHUNK==64
// Base Bits= 56
const BIG_256_56 Modulus_JUBJUB= {0xFFFFFF00000001L,0xA402FFFE5BFEFFL,0x809A1D80553BDL,0x299D7D483339D8L,0x73EDA753L};
const BIG_256_56 ROI_JUBJUB= {0x788F500B912F1FL,0x4FF270B3E0941BL,0xC8D168D6C0C402L,0x5B416B6F0FD56DL,0x212D79EL};
const BIG_256_56 R2modp_JUBJUB= {0x3B3440EC31BBA9L,0x8929657E045FB0L,0x2D645CF57C6E1AL,0xEA6A1C5012ECF5L,0x3C7B9D12L};
const chunk MConst_JUBJUB= 0xFFFFFEFFFFFFFFL;
#endif

