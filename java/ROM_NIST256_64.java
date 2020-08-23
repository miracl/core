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


package org.miracl.core.NIST256;

public class ROM
{

// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFL,0x0L,0x1000000L,0xFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFFFEL,0xFFFFFFFFFFL,0x0L,0x1000000L,0xFFFFFFFFL};
	public static final long[] R2modp= {0x3000000050000L,0x0L,0xFFFFFBFFFFFFFAL,0xFFFAFFFFFFFEFFL,0x2FFFFL};
	public static final long MConst= 0x1L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_B_I= 0;
	public static final long[] CURVE_B= {0xCE3C3E27D2604BL,0x6B0CC53B0F63BL,0x55769886BC651DL,0xAA3A93E7B3EBBDL,0x5AC635D8L};
	public static final long[] CURVE_Order= {0xB9CAC2FC632551L,0xFAADA7179E84F3L,0xFFFFFFFFFFBCE6L,0xFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Gx= {0xA13945D898C296L,0x7D812DEB33A0F4L,0xE563A440F27703L,0xE12C4247F8BCE6L,0x6B17D1F2L};
	public static final long[] CURVE_Gy= {0xB6406837BF51F5L,0x33576B315ECECBL,0x4A7C0F9E162BCEL,0xFE1A7F9B8EE7EBL,0x4FE342E2L};
    public static final long[] CURVE_HTPC= {0xB847959C613926L,0xB001452A9DD42EL,0x8F44A655548D3EL,0xB63723A45240B3L,0x6A2AD82CL};
}

