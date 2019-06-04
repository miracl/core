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


package org.apache.milagro.amcl.NUMS256E;

public class ROM
{
	public static final int[] Modulus= {0x1FFFFF43,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFFFFFF};
	public static final int[] R2modp= {0x22E2400,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	public static final int MConst= 0xBD;

	public static final int CURVE_Cof_I= 4;
	public static final int[] CURVE_Cof= {0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= -15342;
	public static final int[] CURVE_B= {0x1FFFC355,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFFFFFF};
	public static final int[] CURVE_Order= {0xEDD4AF5,0x123D8C87,0x1650E6C6,0xAB54A5E,0x419,0x0,0x0,0x0,0x400000};
	public static final int[] CURVE_Gx= {0xEED13DA,0x6F60481,0x20D61A8,0x13141DC6,0x9BD60C3,0x1EAFB490,0xDF73478,0x1F6D5D44,0x8A7514};
	public static final int[] CURVE_Gy= {0x198A89E6,0x1D30B73B,0x15BB4CB,0x1EC3B021,0x18010715,0x12ECD325,0x171F3A59,0x13FB3B24,0x44D53E};
}
