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


package org.miracl.core.SECP160R1;

public class ROM
{
// Base Bits= 56
public static final long[] Modulus= {0xFFFFFF7FFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
public static final long[] R2modp= {0x1000000010000L,0x400000L,0x0L};
public static final long[] ROI= {0xFFFFFF7FFFFFFEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
public static final long MConst= 0xFFFFFF80000001L;

public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0xD4D4ADC565FA45L,0x7A8B65ACF89F81L,0x1C97BEFC54BDL};
public static final long[] CURVE_Order= {0x27AED3CA752257L,0x1F4C8F9L,0x1000000000000L};
public static final long[] CURVE_Gx= {0xC38BB913CBFC82L,0x73284664698968L,0x4A96B5688EF5L};
public static final long[] CURVE_Gy= {0x2351377AC5FB32L,0x947D59DCC91204L,0x23A628553168L};
public static final long[] CURVE_HTPC= {0xDAFAF3A25BC0DAL,0x6106FACE6A7305L,0x1E50BD02CD5AL};
}

