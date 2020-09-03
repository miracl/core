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
#include "fp_TWEEDLEDUM.h"

/* TWEEDLEDUM Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base  bits= 29
const BIG_256_29 Modulus_TWEEDLEDUM= {0x1,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000};
const BIG_256_29 R2modp_TWEEDLEDUM= {0x3FF4,0x1B622B40,0x1166FB28,0x5BEEDDF,0x16646668,0x321298C,0xDD7B28E,0x3E66507,0x22B644};
const BIG_256_29 ROI_TWEEDLEDUM= {0x188837CE,0x1EF4F98D,0x1AA67594,0xDD1E7E7,0x2D57FF4,0x197557C6,0xBF3D893,0x2F121DC,0x2AE451};
const BIG_256_29 SQRTm3_TWEEDLEDUM= {0x17DE6C70,0x10786BD5,0x1DC95B25,0xD55C069,0x184CB128,0x16EB9107,0x1B5B20A1,0x95285A0,0x15EF7D};
const chunk MConst_TWEEDLEDUM= 0x1FFFFFFF;
#endif

#if CHUNK==64
// Base  bits= 56
const BIG_256_56 Modulus_TWEEDLEDUM= {0x4064E200000001L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
const BIG_256_56 R2modp_TWEEDLEDUM= {0xFC85D749BBE0CDL,0x64ED6614FCA69BL,0x9925C8F6AC3D6DL,0x28E19094C65991L,0xA0EDD7BL};
const BIG_256_56 ROI_TWEEDLEDUM= {0xDE9F31B88837CEL,0xE8F3F3EA99D653L,0xEAAF8C2D57FF46L,0x890EE2FCF624F2L,0x2AE45117L};
const BIG_256_56 SQRTm3_TWEEDLEDUM= {0xF0D7AB7DE6C70L,0xAAE034F7256C96L,0xD7220F84CB1286L,0x942D06D6C8286DL,0x15EF7D4AL};
const chunk MConst_TWEEDLEDUM= 0x4064E1FFFFFFFFL;
#endif

