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


package org.miracl.core.MDC;

public class ROM
{

// Base Bits= 56
public static final long[] Modulus= {0x3710405779EC13L,0x9037EF07AF9EC1L,0xFDD7A5FD4F086AL,0xD456AFB4532F92L,0xF13B68B9L};
public static final long[] R2modp= {0xDE5B77FDA5D1FEL,0x32283852985975L,0xA24A9FC95FA423L,0x6853D0EA869FAEL,0x3EC4B765L};
public static final long[] ROI= {0x3710405779EC12L,0x9037EF07AF9EC1L,0xFDD7A5FD4F086AL,0xD456AFB4532F92L,0xF13B68B9L};
public static final long MConst= 0xF10FDDAAE541E5L;

public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0x9309250300CF89L,0x70F63F21F041EEL,0xCCFB0CB9625F12L,0x1965B68A7CDFBFL,0x57130452L};
public static final long[] CURVE_Order= {0x8552F421B0F7FBL,0x8975FAF974BB58L,0xBF75E97F534FB3L,0x7515ABED14CBE4L,0x3C4EDA2EL};
public static final long[] CURVE_Gx= {0x2C25BFEE68363BL,0x2ABB8D2713E223L,0x1E03CBCF6350D7L,0x7F903B83D85B42L,0xB681886AL};
public static final long[] CURVE_Gy= {0x73C355BD2864B5L,0xBC3D81A93A3A7EL,0xCF6563DA421DA8L,0xB59C0B0359814DL,0xCA6734E1L};
public static final long[] CURVE_HTPC= {0x1L,0x0L,0x0L,0x0L,0x0L};
}

