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


package org.miracl.core.NUMS256W;

public class ROM
{

// Modulus
	public static final long[] Modulus= {0xFFFFFFFFFFFF43L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFF42L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] R2modp= {0x89000000000000L,0x8BL,0x0L,0x0L,0x0L};
	public static final long MConst= 0xBDL;

// Curve

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_B_I= 152961;
	public static final long[] CURVE_B= {0x25581L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xAB20294751A825L,0x8275EA265C6020L,0xFFFFFFFFFFE43CL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x52EE1EB21AACB1L,0x9B0903D4C73ABCL,0xA04F42CB098357L,0x5AAADB61297A95L,0xBC9ED6B6L};
	public static final long[] CURVE_Gy= {0xB5B9CB2184DE9FL,0xC3D115310FBB80L,0xF77E04E035C955L,0x3399B6A673448BL,0xD08FC0F1L};
    public static final long[] CURVE_HTPC= {0xFC6F75952B84D6L,0x92C62040E89E05L,0x70CADDC6AE4640L,0x5411E3B5B22ED0L,0xCF7F44E4L};
}

