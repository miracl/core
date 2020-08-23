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


package org.miracl.core.C1665;

public class ROM
{
// Base Bits= 60
public static final long[] Modulus= {0xFFFFFFFFFFFFFFBL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
public static final long[] R2modp= {0x190000000L,0x0L,0x0L};
public static final long[] ROI= {0xFFFFFFFFFFFFFFAL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
public static final long MConst= 0x5L;

//*** rom curve parameters *****
// Base Bits= 60

public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L};
public static final int CURVE_B_I= 5766;
public static final long[] CURVE_B= {0x1686L,0x0L,0x0L};
public static final long[] CURVE_Order= {0x80FF0A99DBA8B27L,0xFFFFFFFFFD5EF01L,0xFFFFFFFFFFFL};
public static final long[] CURVE_Gx= {0x671B9DBF9D52398L,0x9A0618EE0F666CL,0x14C94DA505B8L};
public static final long[] CURVE_Gy= {0xC7087B244920345L,0x13BFC7739D62DC2L,0x29414549BC0AL};
public static final long[] CURVE_HTPC= {0x1L,0x0L,0x0L};
}

