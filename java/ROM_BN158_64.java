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


package org.miracl.core.BN158;

public class ROM
{
public static final long[] Modulus= {0x72280AB04E013L,0x9953CF6FD3FB95L,0x24012027628CL};
public static final long[] R2modp= {0xA60739B545973DL,0x52616565DB288L,0x1731500B226CL};
public static final long[] ROI= {0x72280AB04E012L,0x9953CF6FD3FB95L,0x24012027628CL};
public static final long[] SQRTm3= {0x7F0E4048018004L,0x95F35CA99F4CE2L,0x2401202761FCL};
public static final long[] CRu= {0x440A203181B007L,0x1B039631A5759L,0x48L};
public static final long MConst= 0xFC609004F615E5L;
public static final long[] Fra= {0xA1674296ECE2A9L,0xE1BF21C99296F2L,0xA85ECF82A02L};
public static final long[] Frb= {0x65BB3E1417FD6AL,0xB794ADA64164A2L,0x197B332F3889L};

//*** rom curve parameters *****
// Ate Bits= 42
// G2 Table size= 49
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L};
public static final int CURVE_B_I= 5;
public static final long[] CURVE_B= {0x5L,0x0L,0x0L};
public static final long[] CURVE_Order= {0xD59F209F04200DL,0x9953CF6F73FA14L,0x24012027628CL};
public static final long[] CURVE_Gx= {0x72280AB04E012L,0x9953CF6FD3FB95L,0x24012027628CL};
public static final long[] CURVE_Gy= {0x2L,0x0L,0x0L};
public static final long[] CURVE_HTPC= {0x1L,0x0L,0x0L};

public static final long[] CURVE_Bnx= {0x4000801001L,0x0L,0x0L};
public static final long[] CURVE_Pxa= {0x3B2765033A5768L,0x1EECE2B3022922L,0x1EA35F882728L};
public static final long[] CURVE_Pxb= {0x7B04ACE776A2F5L,0x5D05BA314F9D68L,0x23485611EB92L};
public static final long[] CURVE_Pya= {0x69AB26E30CFE24L,0x1FB7A85F92C435L,0x1C952F906B6EL};
public static final long[] CURVE_Pyb= {0x91017738E8609DL,0x8445B3BA0F3EE2L,0x23E289544ED8L};
public static final long[][] CURVE_W= {{0x3182600A008003L,0x600180L,0x0L},{0x8001002001L,0x0L,0x0L}};
public static final long[][][] CURVE_SB= {{{0x3182E00B00A004L,0x600180L,0x0L},{0x8001002001L,0x0L,0x0L}},{{0x8001002001L,0x0L,0x0L},{0xA41CC09503A00AL,0x9953CF6F13F894L,0x24012027628CL}}};
public static final long[][] CURVE_WB= {{0x10806002801000L,0x200080L,0x0L},{0xF907C026815005L,0x1202642519090L,0x30L},{0x7C84001380B003L,0x90132128C848L,0x18L},{0x1080E003803001L,0x200080L,0x0L}};
public static final long[][][] CURVE_BB= {{{0xD59EE09E84100DL,0x9953CF6F73FA14L,0x24012027628CL},{0xD59EE09E84100CL,0x9953CF6F73FA14L,0x24012027628CL},{0xD59EE09E84100CL,0x9953CF6F73FA14L,0x24012027628CL},{0x8001002002L,0x0L,0x0L}},{{0x8001002001L,0x0L,0x0L},{0xD59EE09E84100CL,0x9953CF6F73FA14L,0x24012027628CL},{0xD59EE09E84100DL,0x9953CF6F73FA14L,0x24012027628CL},{0xD59EE09E84100CL,0x9953CF6F73FA14L,0x24012027628CL}},{{0x8001002002L,0x0L,0x0L},{0x8001002001L,0x0L,0x0L},{0x8001002001L,0x0L,0x0L},{0x8001002001L,0x0L,0x0L}},{{0x4000801002L,0x0L,0x0L},{0x10002004002L,0x0L,0x0L},{0xD59EA09E04000AL,0x9953CF6F73FA14L,0x24012027628CL},{0x4000801002L,0x0L,0x0L}}};

}

