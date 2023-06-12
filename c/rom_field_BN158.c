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
#include "fp_BN158.h"

/* Curve BN158 - Pairing friendly BN curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 28
const BIG_160_28 Modulus_BN158= {0xB04E013,0x72280A,0xFD3FB95,0x9953CF6,0x27628C,0x24012};
const BIG_160_28 R2modp_BN158= {0x545973D,0xA60739B,0x65DB288,0x526165,0xB226C,0x17315};
const BIG_160_28 ROI_BN158= {0xB04E012,0x72280A,0xFD3FB95,0x9953CF6,0x27628C,0x24012};
const BIG_160_28 SQRTm3_BN158= {0x8018004,0x7F0E404,0x99F4CE2,0x95F35CA,0x2761FC,0x24012};
const BIG_160_28 CRu_BN158= {0x181B007,0x440A203,0x31A5759,0x1B0396,0x48,0x0};
const chunk MConst_BN158= 0x4F615E5;
const BIG_160_28 Fra_BN158= {0x6ECE2A9,0xA167429,0x99296F2,0xE1BF21C,0xCF82A02,0xA85E};
const BIG_160_28 Frb_BN158= {0x417FD6A,0x65BB3E1,0x64164A2,0xB794ADA,0x32F3889,0x197B3};
#endif

#if CHUNK==64
// Base Bits= 56
const BIG_160_56 Modulus_BN158= {0x72280AB04E013L,0x9953CF6FD3FB95L,0x24012027628CL};
const BIG_160_56 R2modp_BN158= {0xA60739B545973DL,0x52616565DB288L,0x1731500B226CL};
const BIG_160_56 ROI_BN158= {0x72280AB04E012L,0x9953CF6FD3FB95L,0x24012027628CL};
const BIG_160_56 SQRTm3_BN158= {0x7F0E4048018004L,0x95F35CA99F4CE2L,0x2401202761FCL};
const BIG_160_56 CRu_BN158= {0x440A203181B007L,0x1B039631A5759L,0x48L};
const chunk MConst_BN158= 0xFC609004F615E5L;
const BIG_160_56 Fra_BN158= {0xA1674296ECE2A9L,0xE1BF21C99296F2L,0xA85ECF82A02L};
const BIG_160_56 Frb_BN158= {0x65BB3E1417FD6AL,0xB794ADA64164A2L,0x197B332F3889L};
#endif

