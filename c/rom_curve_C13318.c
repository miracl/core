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
#include "ecp_C13318.h"

/* C13318 */

#if CHUNK==16
/* C13318 Curve */

#error Not supported

#endif

#if CHUNK==32

const int CURVE_Cof_I_C13318= 1;
const BIG_256_29 CURVE_Cof_C13318= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I_C13318= 13318;
const BIG_256_29 CURVE_B_C13318= {0x3406,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 CURVE_Order_C13318= {0xDC2CBE3,0x1BE896E2,0x1AE345BA,0xCA9F07B,0xF4F,0x0,0x0,0x0,0x800000};
const BIG_256_29 CURVE_Gx_C13318= {0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 CURVE_Gy_C13318= {0xB6EAD0B,0x6469AA3,0x5B6D6E,0x1996099E,0x166369D4,0x18728B34,0x1BC4E058,0x1B24D794,0x6675AA};
const BIG_256_29 CURVE_HTPC_C13318= {0x507504F,0x13B86C9D,0x5FC8F18,0x31806AD,0x1EBD397A,0x264F7E,0x1E16569A,0x804FC1D,0x55C192};
#endif

#if CHUNK==64

const int CURVE_Cof_I_C13318= 1;
const BIG_256_56 CURVE_Cof_C13318= {0x1L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I_C13318= 13318;
const BIG_256_56 CURVE_B_C13318= {0x3406L,0x0L,0x0L,0x0L,0x0L};
const BIG_256_56 CURVE_Order_C13318= {0x7D12DC4DC2CBE3L,0x54F83DEB8D16EBL,0xF4F6L,0x0L,0x80000000L};
const BIG_256_56 CURVE_Gx_C13318= {0x5L,0x0L,0x0L,0x0L,0x0L};
const BIG_256_56 CURVE_Gy_C13318= {0xC8D3546B6EAD0BL,0xCB04CF016DB5B8L,0xE5166966369D4CL,0x26BCA6F1381630L,0x6675AAD9L};
const BIG_256_56 CURVE_HTPC_C13318= {0x770D93A507504FL,0x8C035697F23C62L,0x4C9EFDEBD397A1L,0x27E0EF8595A680L,0x55C19240L};
#endif

