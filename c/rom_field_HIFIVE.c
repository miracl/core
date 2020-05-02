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
#include "fp_HIFIVE.h"

/* Curve HIFIVE */


#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
// Base Bits= 29
const BIG_336_29 Modulus_HIFIVE= {0x1FFFFFFD,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFF};
const BIG_336_29 ROI_HIFIVE= {0x1559D3B8,0x1C02413F,0xFAB18DA,0x128A7172,0xB95DA4C,0x16D4FE54,0x3107D87,0xAA7BEF3,0x1C38B2B4,0x1A93C08F,0x10F80C7B,0x1F27F};
const BIG_336_29 R2modp_HIFIVE= {0x9000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const chunk MConst_HIFIVE= 0x3;
#endif

#if CHUNK==64
// Base Bits= 60
const BIG_336_60 Modulus_HIFIVE= {0xFFFFFFFFFFFFFFDL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFL};
const BIG_336_60 ROI_HIFIVE= {0xB804827F559D3B8L,0xC94538B93EAC636L,0x1EDA9FCA8B95DA4L,0xB4553DF798C41F6L,0x1EF527811FC38B2L,0xF93FC3E03L};
const BIG_336_60 R2modp_HIFIVE= {0x9000000000000L,0x0L,0x0L,0x0L,0x0L,0x0L};
const chunk MConst_HIFIVE= 0x3L;
#endif


