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
#include "ecp_SM2.h"

/* Curve SM2 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

// Base Bits= 28

const int CURVE_Cof_I_SM2= 1;
const BIG_256_28 CURVE_Cof_SM2= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I_SM2= 0;
const BIG_256_28 CURVE_B_SM2= {0xD940E93,0xBCBD414,0xB8F92DD,0x89F515A,0x9A7F397,0x4BCF650,0x44D5A9E,0x9D9F5E3,0x8E9FA9E,0x2};
const BIG_256_28 CURVE_Order_SM2= {0x9D54123,0xBBF4093,0x6052B53,0xDF6B21C,0xFFF7203,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF};
const BIG_256_28 CURVE_Gx_SM2= {0x34C74C7,0x5A45893,0x60BE171,0xBBFF26,0x9948FE3,0x466A39C,0x95F9904,0x1F19811,0x2C4AE2C,0x3};
const BIG_256_28 CURVE_Gy_SM2= {0x139F0A0,0xDF32E52,0xA474002,0x877CC62,0x153D0A9,0xE36B692,0xC59BDCE,0xF4F6779,0xC3736A2,0xB};
const BIG_256_28 CURVE_HTPC_SM2= {0xFFFFFFF,0xFFFFFFF,0x55555FF,0xFFFF555,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xAAAAAA9,0xA};
#endif

#if CHUNK==64

// Base Bits= 56

const int CURVE_Cof_I_SM2= 1;
const BIG_256_56 CURVE_Cof_SM2= {0x1L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I_SM2= 0;
const BIG_256_56 CURVE_B_SM2= {0xBCBD414D940E93L,0x89F515AB8F92DDL,0x4BCF6509A7F397L,0x9D9F5E344D5A9EL,0x28E9FA9EL};
const BIG_256_56 CURVE_Order_SM2= {0xBBF40939D54123L,0xDF6B21C6052B53L,0xFFFFFFFFFF7203L,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
const BIG_256_56 CURVE_Gx_SM2= {0x5A4589334C74C7L,0xBBFF2660BE171L,0x466A39C9948FE3L,0x1F1981195F9904L,0x32C4AE2CL};
const BIG_256_56 CURVE_Gy_SM2= {0xDF32E52139F0A0L,0x877CC62A474002L,0xE36B692153D0A9L,0xF4F6779C59BDCEL,0xBC3736A2L};
const BIG_256_56 CURVE_HTPC_SM2= {0xFFFFFFFFFFFFFFL,0xFFFF55555555FFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xAAAAAAA9L};
#endif

