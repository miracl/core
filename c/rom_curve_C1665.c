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
#include "ecp_C1665.h"

/* Curve C1665 */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32

const int CURVE_Cof_I_C1665= 4;
const BIG_168_29 CURVE_Cof_C1665= {0x4,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I_C1665= 5766;
const BIG_168_29 CURVE_B_C1665= {0x1686,0x0,0x0,0x0,0x0,0x0};
const BIG_168_29 CURVE_Order_C1665= {0x1DBA8B27,0x7F854C,0x1F57BC06,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
const BIG_168_29 CURVE_Gx_C1665= {0x19D52398,0x138DCEDF,0x183D99B1,0x1340C31D,0x1A505B80,0xA64A6};
const BIG_168_29 CURVE_Gy_C1665= {0x4920345,0x3843D92,0x758B70B,0x77F8EE7,0x149BC0A1,0x14A0A2};
const BIG_168_29 CURVE_HTPC_C1665= {0x1,0x0,0x0,0x0,0x0,0x0};
#endif

#if CHUNK==64

const int CURVE_Cof_I_C1665= 4;
const BIG_168_60 CURVE_Cof_C1665= {0x4L,0x0L,0x0L};
const int CURVE_B_I_C1665= 5766;
const BIG_168_60 CURVE_B_C1665= {0x1686L,0x0L,0x0L};
const BIG_168_60 CURVE_Order_C1665= {0x80FF0A99DBA8B27L,0xFFFFFFFFFD5EF01L,0xFFFFFFFFFFFL};
const BIG_168_60 CURVE_Gx_C1665= {0x671B9DBF9D52398L,0x9A0618EE0F666CL,0x14C94DA505B8L};
const BIG_168_60 CURVE_Gy_C1665= {0xC7087B244920345L,0x13BFC7739D62DC2L,0x29414549BC0AL};
const BIG_168_60 CURVE_HTPC_C1665= {0x1L,0x0L,0x0L};
#endif

