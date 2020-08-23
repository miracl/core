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


package org.miracl.core.NUMS256E;

public class ROM
{
	public static final long[] Modulus= {0xFFFFFFFFFFFF43L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFF42L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] R2modp= {0x89000000000000L,0x8BL,0x0L,0x0L,0x0L};
	public static final long MConst= 0xBDL;

	public static final int CURVE_Cof_I= 4;
	public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_B_I= -15342;
	public static final long[] CURVE_B= {0xFFFFFFFFFFC355L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Order= {0x47B190EEDD4AF5L,0x5AA52F59439B1AL,0x4195L,0x0L,0x40000000L};
	public static final long[] CURVE_Gx= {0xDEC0902EED13DAL,0x8A0EE3083586A0L,0x5F69209BD60C39L,0x6AEA237DCD1E3DL,0x8A7514FBL};
	public static final long[] CURVE_Gy= {0xA616E7798A89E6L,0x61D810856ED32FL,0xD9A64B8010715FL,0xD9D925C7CE9665L,0x44D53E9FL};
    public static final long[] CURVE_HTPC= {0x1L,0x0L,0x0L,0x0L,0x0L};

}

