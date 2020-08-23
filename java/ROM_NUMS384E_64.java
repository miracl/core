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


package org.miracl.core.NUMS384E;

public class ROM
{
// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFEC3L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFEC2L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] R2modp= {0x188890000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x13DL;

	public static final int CURVE_Cof_I= 4;
	public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_B_I= -11556;
	public static final long[] CURVE_B= {0xFFFFFFFFFFD19FL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] CURVE_Order= {0xB9DCC4E6A3897DL,0x555AAB35C87920L,0x1CB46BE1CF61E4L,0xFFFFFFFFE2471AL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0xD540E46C206BDEL,0x92B16545941350L,0xA8F33163406FF2L,0xE5BE4C005763FFL,0xE55DB5B30BF446L,0x266CC0B6A2129AL,0x61B111FB45A9L};
	public static final long[] CURVE_Gy= {0x8D03E1F0729392L,0xB0F946EC48DC9DL,0xF7F645964B0072L,0xF1425F56830F98L,0xB10DD716AD8274L,0xEEB08738B1A423L,0x82983E67B9A6L};
    public static final long[] CURVE_HTPC= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
}
