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

/* Fixed Data in ROM - Field and Curve parameters */


package org.miracl.core.NUMS512W;

public class ROM
{

// Base  bits= 60
public static final long[] Modulus= {0xFFFFFFFFFFFFDC7L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] R2modp= {0x100000000000000L,0x4F0BL,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
public static final long[] ROI= {0xFFFFFFFFFFFFDC6L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long MConst= 0x239L;

//*** rom curve parameters *****
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 121243;
public static final long[] CURVE_B= {0x1D99BL,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0xE153F390433555DL,0x568B36607CD243CL,0x258ED97D0BDC63BL,0xA4FB94E7831B4FCL,0xFFFFFFFFFFF5B3CL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] CURVE_Gx= {0xC8287958CABAE57L,0x5D60137D6F5DE2DL,0x94286255615831DL,0xA151076B359E937L,0xC25306D9F95021L,0x3BB501F6854506EL,0x2A03D3B5298CAD8L,0x141D0A93DA2B700L,0x3AC03447L};
public static final long[] CURVE_Gy= {0x3A08760383527A6L,0x2B5C1E4CFD0FE92L,0x1A840B25A5602CFL,0x15DA8B0EEDE9C12L,0x60C7BD14F14A284L,0xDEABBCBB8C8F4B2L,0xC63EBB1004B97DBL,0x29AD56B3CE0EEEDL,0x943A54CAL};
public static final long[] CURVE_HTPC= {0xFFFFFFFFFFFFEE3L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x7FFFFFFFL};

}
