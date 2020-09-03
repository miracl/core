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


package org.miracl.core.TWEEDLEDUM;

public class ROM
{

// Base  bits= 56
public static final long[] Modulus= {0x4064E200000001L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
public static final long[] R2modp= {0xFC85D749BBE0CDL,0x64ED6614FCA69BL,0x9925C8F6AC3D6DL,0x28E19094C65991L,0xA0EDD7BL};
public static final long[] ROI= {0xDE9F31B88837CEL,0xE8F3F3EA99D653L,0xEAAF8C2D57FF46L,0x890EE2FCF624F2L,0x2AE45117L};
public static final long[] SQRTm3= {0xF0D7AB7DE6C70L,0xAAE034F7256C96L,0xD7220F84CB1286L,0x942D06D6C8286DL,0x15EF7D4AL};
public static final long MConst= 0x4064E1FFFFFFFFL;

//*** rom curve parameters *****
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 5;
public static final long[] CURVE_B= {0x5L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0x2CAFD400000001L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
public static final long[] CURVE_Gx= {0x4064E200000000L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
public static final long[] CURVE_Gy= {0x2L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_HTPC= {0xD6755341381795L,0xAEEBD03CFBC366L,0x8B6CAABAA53E10L,0x37020BF4EF14D8L,0x3AB46CF3L};
}

