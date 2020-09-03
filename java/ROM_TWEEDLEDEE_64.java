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


package org.miracl.core.TWEEDLEDEE;

public class ROM
{

// Base  bits= 56
public static final long[] Modulus= {0x2CAFD400000001L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
public static final long[] R2modp= {0x21CA8949BBE5DFL,0x717E3D9E719200L,0xB8A93694FCEE5L,0xE6FCB03AA0A27EL,0x2013F4C0L};
public static final long[] ROI= {0xBD3234869D57ECL,0x7DD1B8084FBF1CL,0x645A9872930A28L,0xDC03C0BF1DBCBL,0x113EFC51L};
public static final long[] SQRTm3= {0xFCD5A6291F49FAL,0x7AA6D867D6B902L,0xBFF2E63FFCD014L,0x3C0934B10F5B16L,0x2D8CDA74L};
public static final long MConst= 0x2CAFD3FFFFFFFFL;

//*** rom curve parameters *****
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 5;
public static final long[] CURVE_B= {0x5L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0x4064E200000001L,0xA1276C3F59B9A1L,0x38AL,0x0L,0x40000000L};
public static final long[] CURVE_Gx= {0x2CAFD400000000L,0xA127696286C984L,0x38AL,0x0L,0x40000000L};
public static final long[] CURVE_Gy= {0x2L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_HTPC= {0x861CBDAD69C848L,0x754672EF68733L,0xE5BC1A0E9704ACL,0x2A64972082C6A2L,0x3AC188DEL};

}

