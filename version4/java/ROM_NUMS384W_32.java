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

// Base Bits= 29
	public static final int[] Modulus= {0x1FFFFEC3,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
	public static final int[] R2modp= {0x0,0x4448000,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	public static final int MConst= 0x13D;

	public static final int CURVE_Cof_I= 1;
	public static final int[] CURVE_Cof= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= -34568;
	public static final int[] CURVE_B= {0x1FFF77BB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
	public static final int[] CURVE_Order= {0x1B0E61B9,0x26C0FB3,0xDF89E98,0x153A7A98,0x16881BED,0x178F75AE,0x1FFF587A,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F};
	public static final int[] CURVE_Gx= {0x98152A,0x1CE5D021,0x18711EFA,0x1DDA201E,0xC742522,0x148D9536,0x7D3CEF4,0x19BF703F,0x60225C1,0x12082F8D,0x12203288,0x2DE3038,0x17956F0B,0x3A};
	public static final int[] CURVE_Gy= {0x6180716,0x3A5C763,0x1D2B4997,0xD69B77F,0x837EBCD,0x1BE890D,0xE72E482,0xEFF0FEE,0x1EB00469,0x2C267B,0x15F8CF4C,0x3371C71,0xDEE368E,0x56};


}
