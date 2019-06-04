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


package org.apache.milagro.amcl.BN254;

public class ROM
{

	public static final long[] Modulus= {0x13L,0x13A7L,0x80000000086121L,0x40000001BA344DL,0x25236482L};
	public static final long[] R2modp= {0x2F2A96FF5E7E39L,0x64E8642B96F13CL,0x9926F7B00C7146L,0x8321E7B4DACD24L,0x1D127A2EL};
	public static final long MConst= 0x435E50D79435E5L;

	public static final int CURVE_Cof_I= 1;
	public static final int CURVE_A= 0;
	public static final int CURVE_B_I= 2;
	public static final long[] CURVE_B= {0x2L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xDL,0x800000000010A1L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L};
	public static final long[] CURVE_Gx= {0x12L,0x13A7L,0x80000000086121L,0x40000001BA344DL,0x25236482L};
	public static final long[] CURVE_Gy= {0x1L,0x0L,0x0L,0x0L,0x0L};

	public static final long[] CURVE_Bnx= {0x80000000000001L,0x40L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Cru= {0x80000000000007L,0x6CDL,0x40000000024909L,0x49B362L,0x0L};
	public static final long[] Fra= {0x7DE6C06F2A6DE9L,0x74924D3F77C2E1L,0x50A846953F8509L,0x212E7C8CB6499BL,0x1B377619L};
	public static final long[] Frb= {0x82193F90D5922AL,0x8B6DB2C08850C5L,0x2F57B96AC8DC17L,0x1ED1837503EAB2L,0x9EBEE69L};
	public static final long[] CURVE_Pxa= {0xEE4224C803FB2BL,0x8BBB4898BF0D91L,0x7E8C61EDB6A464L,0x519EB62FEB8D8CL,0x61A10BBL};
	public static final long[] CURVE_Pxb= {0x8C34C1E7D54CF3L,0x746BAE3784B70DL,0x8C5982AA5B1F4DL,0xBA737833310AA7L,0x516AAF9L};
	public static final long[] CURVE_Pya= {0xF0E07891CD2B9AL,0xAE6BDBE09BD19L,0x96698C822329BDL,0x6BAF93439A90E0L,0x21897A0L};
	public static final long[] CURVE_Pyb= {0x2D1AEC6B3ACE9BL,0x6FFD739C9578AL,0x56F5F38D37B090L,0x7C8B15268F6D44L,0xEBB2B0EL};
	public static final long[][] CURVE_W= {{0x3L,0x80000000000204L,0x6181L,0x0L,0x0L},{0x1L,0x81L,0x0L,0x0L,0x0L}};
	public static final long[][][] CURVE_SB= {{{0x4L,0x80000000000285L,0x6181L,0x0L,0x0L},{0x1L,0x81L,0x0L,0x0L,0x0L}},{{0x1L,0x81L,0x0L,0x0L,0x0L},{0xAL,0xE9DL,0x80000000079E1EL,0x40000001BA344DL,0x25236482L}}};
	public static final long[][] CURVE_WB= {{0x80000000000000L,0x80000000000040L,0x2080L,0x0L,0x0L},{0x80000000000005L,0x54AL,0x8000000001C707L,0x312241L,0x0L},{0x80000000000003L,0x800000000002C5L,0xC000000000E383L,0x189120L,0x0L},{0x80000000000001L,0x800000000000C1L,0x2080L,0x0L,0x0L}};
	public static final long[][][] CURVE_BB= {{{0x8000000000000DL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x8000000000000CL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x8000000000000CL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x2L,0x81L,0x0L,0x0L,0x0L}},{{0x1L,0x81L,0x0L,0x0L,0x0L},{0x8000000000000CL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x8000000000000DL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x8000000000000CL,0x80000000001060L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L}},{{0x2L,0x81L,0x0L,0x0L,0x0L},{0x1L,0x81L,0x0L,0x0L,0x0L},{0x1L,0x81L,0x0L,0x0L,0x0L},{0x1L,0x81L,0x0L,0x0L,0x0L}},{{0x80000000000002L,0x40L,0x0L,0x0L,0x0L},{0x2L,0x102L,0x0L,0x0L,0x0L},{0xAL,0x80000000001020L,0x8000000007FF9FL,0x40000001BA344DL,0x25236482L},{0x80000000000002L,0x40L,0x0L,0x0L,0x0L}}};

}

