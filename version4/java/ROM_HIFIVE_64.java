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


package org.apache.milagro.amcl.HIFIVE;

public class ROM
{

// Base Bits= 60
	public static final long[] Modulus= {0xFFFFFFFFFFFFFFDL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] R2modp= {0x9000000000000L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x3L;

	public static final int CURVE_Cof_I= 8;
	public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= 11111;
	public static final long[] CURVE_B= {0x2B67L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xB2F95973E9FA805L,0xC0BD6B87F93BAA7L,0x71415FA9850L,0x0L,0x0L,0x200000000L};
	public static final long[] CURVE_Gx= {0xCL,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Gy= {0x2BEC68505FE8632L,0x5D5650CA0365DB1L,0x3811C7EF435B6DBL,0x7853D1B14B46CL,0x56502E18E1C161DL,0xC0DC616BL};

}

