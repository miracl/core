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
#include "fp_MDC.h"

/* Million Dollar Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 28
const BIG_256_28 Modulus_MDC= {0x779EC13,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF};
const BIG_256_28 R2modp_MDC= {0xDA5D1FE,0xDE5B77F,0x2985975,0x3228385,0x95FA423,0xA24A9FC,0xA869FAE,0x6853D0E,0xEC4B765,0x3};
const BIG_256_28 ROI_MDC= {0x779EC12,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF};
const chunk MConst_MDC= 0xAE541E5;
#endif

#if CHUNK==64
// Base Bits= 56
const BIG_256_56 Modulus_MDC= {0x3710405779EC13L,0x9037EF07AF9EC1L,0xFDD7A5FD4F086AL,0xD456AFB4532F92L,0xF13B68B9L};
const BIG_256_56 R2modp_MDC= {0xDE5B77FDA5D1FEL,0x32283852985975L,0xA24A9FC95FA423L,0x6853D0EA869FAEL,0x3EC4B765L};
const BIG_256_56 ROI_MDC= {0x3710405779EC12L,0x9037EF07AF9EC1L,0xFDD7A5FD4F086AL,0xD456AFB4532F92L,0xF13B68B9L};
const chunk MConst_MDC= 0xF10FDDAAE541E5L;
#endif

