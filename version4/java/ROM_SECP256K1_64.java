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


package org.apache.milagro.amcl.SECP256K1;

public class ROM
{

// Base Bits= 56
public static final long[] Modulus= {0xFFFFFEFFFFFC2FL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] R2modp= {0xA1000000000000L,0x7A2000E90L,0x1L,0x0L,0x0L};
public static final long MConst= 0x38091DD2253531L;

public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_A= 0;
public static final int CURVE_B_I= 7;
public static final long[] CURVE_B= {0x7L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0xD25E8CD0364141L,0xDCE6AF48A03BBFL,0xFFFFFFFFFEBAAEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] CURVE_Gx= {0xF2815B16F81798L,0xFCDB2DCE28D959L,0x95CE870B07029BL,0xF9DCBBAC55A062L,0x79BE667EL};
public static final long[] CURVE_Gy= {0x47D08FFB10D4B8L,0xB448A68554199CL,0xFC0E1108A8FD17L,0x26A3C4655DA4FBL,0x483ADA77L};

}

