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


package org.apache.milagro.amcl.ED25519;

public class ROM
{

public static final long[] Modulus= {0xFFFFFFFFFFFFEDL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFFL};
public static final long[] R2modp= {0xA4000000000000L,0x5L,0x0L,0x0L,0x0L};
public static final long MConst= 0x13;


public static final int CURVE_Cof_I= 8;
public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_A= -1;
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0xEB4DCA135978A3L,0xA4D4141D8AB75L,0x797779E8980070L,0x2B6FFE738CC740L,0x52036CEEL};
public static final long[] CURVE_Order= {0x12631A5CF5D3EDL,0xF9DEA2F79CD658L,0x14DEL,0x0L,0x10000000L};
public static final long[] CURVE_Gx= {0x562D608F25D51AL,0xC7609525A7B2C9L,0x31FDD6DC5C692CL,0xCD6E53FEC0A4E2L,0x216936D3L};
public static final long[] CURVE_Gy= {0x66666666666658L,0x66666666666666L,0x66666666666666L,0x66666666666666L,0x66666666L};

}

