/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* Fixed Data in ROM - Field and Curve parameters */


package org.apache.milagro.amcl.NUMS384W;

public class ROM
{
/*
// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFEC3L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] R2modp= {0x188890000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x13DL;

	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= -34568;
	public static final long[] CURVE_B= {0xFFFFFFFFFF77BBL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] CURVE_Order= {0x4D81F67B0E61B9L,0x9D3D4C37E27A60L,0x1EEB5D6881BEDAL,0xFFFFFFFFD61EAFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x9CBA042098152AL,0xED100F61C47BEBL,0x1B2A6CC742522EL,0xFB81F9F4F3BD29L,0x5F1A60225C1CDL,0x181C4880CA2241L,0x757956F0B16FL};
	public static final long[] CURVE_Gy= {0x74B8EC66180716L,0xB4DBBFF4AD265CL,0x7D121A837EBCD6L,0xF87F739CB92083L,0x84CF7EB0046977L,0x8E38D7E33D3005L,0xACDEE368E19BL};
*/
// Base Bits= 58
	public static final long[] Modulus= {0x3FFFFFFFFFFFEC3L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] R2modp= {0x88900000000000L,0x6L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x13DL;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= -34568;
	public static final long[] CURVE_B= {0x3FFFFFFFFFF77BBL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] CURVE_Order= {0x4D81F67B0E61B9L,0x2A74F530DF89E98L,0x2F1EEB5D6881BEDL,0x3FFFFFFFFFF587AL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x39CBA042098152AL,0x3BB4403D8711EFAL,0x291B2A6CC742522L,0x337EE07E7D3CEF4L,0x24105F1A60225C1L,0x5BC60712203288L,0x757956F0BL};
	public static final long[] CURVE_Gy= {0x74B8EC66180716L,0x1AD36EFFD2B4997L,0x37D121A837EBCDL,0x1DFE1FDCE72E482L,0x584CF7EB00469L,0x66E38E35F8CF4CL,0xACDEE368EL};


}
