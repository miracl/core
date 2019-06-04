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


package org.apache.milagro.amcl.NUMS512E;

public class ROM
{
// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFDC7L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFL};
	public static final long[] R2modp= {0x0L,0xF0B10000000000L,0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x239L;

	public static final int CURVE_Cof_I= 4;
	public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= -78296;
	public static final long[] CURVE_B= {0xFFFFFFFFFECBEFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFL};
	public static final long[] CURVE_Order= {0x468CF51BEED46DL,0x5786DEFECFF67L,0xC970B686F52A46L,0x2FCF91BA9E3FD8L,0xFFFFFFB4F0636DL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x3FL};
	public static final long[] CURVE_Gx= {0xB9AB2999EC57FEL,0x25427CC4F015C5L,0x92568904AD0FE5L,0xEE46730F78BDC9L,0x3B81474621C14EL,0xA38227A17EBE27L,0x332FD1E79F4DC4L,0x7A18CB7888D3C5L,0x8E316D128DB69CL,0xDFL};
	public static final long[] CURVE_Gy= {0x6DDEC0C1E2F5E1L,0xD38A9BF1D01F32L,0x862AECC1FD0266L,0xE9963562601A06L,0x9E834120CA53F2L,0x9D22A92B6B9590L,0x6EE476F726D825L,0x98B0F577A82A25L,0x9BFF39D49CA71L,0x6DL};
}
