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


package org.miracl.core.JUBJUB;

public class ROM
{

//*** rom field parameters*****
// Base Bits= 56
public static final long[] Modulus= {0xFFFFFF00000001L,0xA402FFFE5BFEFFL,0x809A1D80553BDL,0x299D7D483339D8L,0x73EDA753L};
public static final long[] ROI= {0x788F500B912F1FL,0x4FF270B3E0941BL,0xC8D168D6C0C402L,0x5B416B6F0FD56DL,0x212D79EL};
public static final long[] R2modp= {0x3B3440EC31BBA9L,0x8929657E045FB0L,0x2D645CF57C6E1AL,0xEA6A1C5012ECF5L,0x3C7B9D12L};

public static final long MConst= 0xFFFFFEFFFFFFFFL;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_Cof_I= 8;
public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0x65FD6D6343EB1L,0x7F6D37579D2601L,0x7E6BD7FD4292DL,0x4BFA2B48F5FD92L,0x2A9318E7L};
public static final long[] CURVE_Order= {0x970E5ED6F72CB7L,0x2093CCC81082D0L,0x101343B00A668L,0x6533AFA906673BL,0xE7DB4EAL};
public static final long[] CURVE_Gx= {0x4ECF1A74F976C4L,0x546BF2F39EDE7FL,0xDF00384882000CL,0xF8EFF38CA624B4L,0x5183972AL};
public static final long[] CURVE_Gy= {0x6BAD709349702EL,0x8707FFA6833B14L,0x5ABD9DC308096CL,0x2CA2FC2C9E8FCCL,0x3B43F847L};
public static final long[] CURVE_HTPC= {0x7AEF9E47BBBB6L,0xFF515175320A99L,0xCB473CD546E25DL,0x2B37B1D403E3EEL,0x62FD68B4L};
}

