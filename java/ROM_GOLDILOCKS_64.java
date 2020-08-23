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


package org.miracl.core.GOLDILOCKS;

public class ROM
{

// Base Bits= 58
public static final long[] Modulus= {0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
public static final long[] ROI= {0x3FFFFFFFFFFFFFEL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
public static final long[] R2modp= {0x200000000L,0x0L,0x0L,0x0L,0x3000000L,0x0L,0x0L,0x0L};
public static final long MConst= 0x1L;


public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= -39081;
public static final long[] CURVE_B= {0x3FFFFFFFFFF6756L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
public static final long[] CURVE_Order= {0x378C292AB5844F3L,0x3309CA37163D548L,0x1B49AED63690216L,0x3FDF3288FA7113BL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFFL};
public static final long[] CURVE_Gx= {0x155555555555555L,0x155555555555555L,0x155555555555555L,0x2A5555555555555L,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAL};
public static final long[] CURVE_Gy= {0x2EAFBCDEA9386EDL,0x32CAFB473681AF6L,0x25833A2A3098BBBL,0x1CA2B6312E03595L,0x35884DD7B7E36DL,0x21B0AC00DBB5E8L,0x17048DB359D6205L,0x2B817A58D2BL};
public static final long[] CURVE_HTPC= {0x3FFFFFFFFFFFFFEL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
}

