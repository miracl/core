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
#include "ecp_TWEEDLEDEE.h"

/* TWEEDLEDUM Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
//*** rom curve parameters *****
const int CURVE_Cof_I_TWEEDLEDEE= 1;
const BIG_256_29 CURVE_Cof_TWEEDLEDEE= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I_TWEEDLEDEE= 5;
const BIG_256_29 CURVE_B_TWEEDLEDEE= {0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 CURVE_Order_TWEEDLEDEE= {0x1,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000};
const BIG_256_29 CURVE_Gx_TWEEDLEDEE= {0x0,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000};
const BIG_256_29 CURVE_Gy_TWEEDLEDEE= {0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const BIG_256_29 CURVE_HTPC_TWEEDLEDEE= {0xD69C848,0x1C30E5ED,0xBBDA1CC,0x180EA8CE,0xE9704A,0x1172DE0D,0x1C820B1A,0x1BC54C92,0x3AC188};
#endif

#if CHUNK==64
//*** rom curve parameters *****
const int CURVE_Cof_I_TWEEDLEDEE= 1;
const BIG_256_56 CURVE_Cof_TWEEDLEDEE= {0x1L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I_TWEEDLEDEE= 5;
const BIG_256_56 CURVE_B_TWEEDLEDEE= {0x5L,0x0L,0x0L,0x0L,0x0L};
const BIG_256_56 CURVE_Order_TWEEDLEDEE= {0x4064E200000001L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
const BIG_256_56 CURVE_Gx_TWEEDLEDEE= {0x2CAFD400000000L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
const BIG_256_56 CURVE_Gy_TWEEDLEDEE= {0x2L,0x0L,0x0L,0x0L,0x0L};
const BIG_256_56 CURVE_HTPC_TWEEDLEDEE= {0x861CBDAD69C848L,0x754672EF68733L,0xE5BC1A0E9704ACL,0x2A64972082C6A2L,0x3AC188DEL};
#endif

