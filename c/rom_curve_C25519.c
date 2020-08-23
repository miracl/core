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
#include "ecp_C25519.h"

/* Curve 25519 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
const int CURVE_Cof_I_C25519=8;
const BIG_256_29 CURVE_Cof_C25519= {0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 CURVE_Order_C25519= {0x1CF5D3ED,0x9318D2,0x1DE73596,0x1DF3BD45,0x14D,0x0,0x0,0x0,0x100000};
const BIG_256_29 CURVE_Gx_C25519= {0x9};
const BIG_256_29 CURVE_HTPC_C25519= {0x507504F,0x13B86C9D,0x5FC8F18,0x31806AD,0x1EBD397A,0x264F7E,0x1E16569A,0x804FC1D,0x55C192};
#endif

#if CHUNK==64
const int CURVE_Cof_I_C25519=8;
const BIG_256_56 CURVE_Cof_C25519= {0x8,0x0,0x0,0x0,0x0};
const BIG_256_56 CURVE_Order_C25519= {0x12631A5CF5D3ED,0xF9DEA2F79CD658,0x14DE,0x0,0x10000000};
const BIG_256_56 CURVE_Gx_C25519= {0x9};
const BIG_256_56 CURVE_HTPC_C25519= {0x770D93A507504FL,0x8C035697F23C62L,0x4C9EFDEBD397A1L,0x27E0EF8595A680L,0x55C19240L};
#endif

