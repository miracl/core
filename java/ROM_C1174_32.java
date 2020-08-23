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


package org.miracl.core.C1174;

public class ROM
{

// Base Bits= 29
public static final int[] Modulus= {0x1FFFFFF7,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
public static final int[] R2modp= {0x5100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
public static final int[] ROI= {0x1FFFFFF6,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
public static final int MConst= 0x9;

//*** rom curve parameters *****
// Base Bits= 29

public static final int CURVE_Cof_I= 4;
public static final int[] CURVE_Cof= {0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
public static final int CURVE_B_I= -1174;
public static final int[] CURVE_B= {0x1FFFFB61,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
public static final int[] CURVE_Order= {0x1166C971,0xA26A2FE,0x14C1CD22,0x12CB89BF,0x1FFFFF77,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFF};
public static final int[] CURVE_Gx= {0x1CE29EDA,0x1091F93D,0xBB35945,0x3B2D492,0x1A190C02,0x1773E014,0x111E4D0E,0x19D4611,0x37FBB};
public static final int[] CURVE_Gy= {0x1B46360E,0x6658DFC,0xFE5DAE9,0x5BDC55E,0xE0C4FE,0x208B4C2,0x1F31995A,0x105A8FF6,0x6B72F};
public static final int[] CURVE_HTPC= {0x1FFFFFF6,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF};
}
