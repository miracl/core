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

// Base Bits= 56
public static final long[] Modulus= {0xFFFFFFFFFFFFF7L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long[] R2modp= {0x0L,0x144L,0x0L,0x0L,0x0L};
public static final long[] ROI= {0xFFFFFFFFFFFFF6L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long MConst= 0x9L;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= -1174;
public static final long[] CURVE_B= {0xFFFFFFFFFFFB61L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long[] CURVE_Order= {0x44D45FD166C971L,0x65C4DFD3073489L,0xFFFFFFFFFFF779L,0xFFFFFFFFFFFFFFL,0x1FFFFFFL};
public static final long[] CURVE_Gx= {0x123F27BCE29EDAL,0xD96A492ECD6516L,0xE7C029A190C021L,0xEA308C479343AEL,0x37FBB0CL};
public static final long[] CURVE_Gy= {0xCCB1BF9B46360EL,0xDEE2AF3F976BA4L,0x1169840E0C4FE2L,0xD47FB7CC665684L,0x6B72F82L};
public static final long[] CURVE_HTPC= {0xFFFFFFFFFFFFF6L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
}

