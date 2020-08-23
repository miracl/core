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
#include "ecp_MDC.h"

/* Million Dollar Curve */

#if CHUNK==16

#error Not supported

#endif

#if CHUNK==32
const int CURVE_Cof_I_MDC= 4;
const BIG_256_28 CURVE_Cof_MDC= {0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
const int CURVE_B_I_MDC= 0;
const BIG_256_28 CURVE_B_MDC= {0x300CF89,0x9309250,0x1F041EE,0x70F63F2,0x9625F12,0xCCFB0CB,0xA7CDFBF,0x1965B68,0x7130452,0x5};
const BIG_256_28 CURVE_Order_MDC= {0x1B0F7FB,0x8552F42,0x974BB58,0x8975FAF,0xF534FB3,0xBF75E97,0xD14CBE4,0x7515ABE,0xC4EDA2E,0x3};
const BIG_256_28 CURVE_Gx_MDC= {0xE68363B,0x2C25BFE,0x713E223,0x2ABB8D2,0xF6350D7,0x1E03CBC,0x3D85B42,0x7F903B8,0x681886A,0xB};
const BIG_256_28 CURVE_Gy_MDC= {0xD2864B5,0x73C355B,0x93A3A7E,0xBC3D81A,0xA421DA8,0xCF6563D,0x359814D,0xB59C0B0,0xA6734E1,0xC};
const BIG_256_28 CURVE_HTPC_MDC= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
#endif

#if CHUNK==64
const int CURVE_Cof_I_MDC= 4;
const BIG_256_56 CURVE_Cof_MDC= {0x4L,0x0L,0x0L,0x0L,0x0L};
const int CURVE_B_I_MDC= 0;
const BIG_256_56 CURVE_B_MDC= {0x9309250300CF89L,0x70F63F21F041EEL,0xCCFB0CB9625F12L,0x1965B68A7CDFBFL,0x57130452L};
const BIG_256_56 CURVE_Order_MDC= {0x8552F421B0F7FBL,0x8975FAF974BB58L,0xBF75E97F534FB3L,0x7515ABED14CBE4L,0x3C4EDA2EL};
const BIG_256_56 CURVE_Gx_MDC= {0x2C25BFEE68363BL,0x2ABB8D2713E223L,0x1E03CBCF6350D7L,0x7F903B83D85B42L,0xB681886AL};
const BIG_256_56 CURVE_Gy_MDC= {0x73C355BD2864B5L,0xBC3D81A93A3A7EL,0xCF6563DA421DA8L,0xB59C0B0359814DL,0xCA6734E1L};
const BIG_256_56 CURVE_HTPC_MDC= {0x1L,0x0L,0x0L,0x0L,0x0L};
#endif

