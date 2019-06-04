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


package org.apache.milagro.amcl.ANSSI;

public class ROM
{

// Base Bits= 56
	public static final long[] Modulus= {0xFCF353D86E9C03L,0xADBCABC8CA6DE8L,0xE8CE42435B3961L,0xB3AD58F10126DL,0xF1FD178CL};
	public static final long[] R2modp= {0x18D2374288CC9CL,0x4929E67646BD2BL,0x220E6C1D6F7F2DL,0x751B1FDABCE02EL,0xE7401B78L};
	public static final long MConst= 0x97483A164E1155L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 0;
	public static final long[] CURVE_B= {0x75ED967B7BB73FL,0xC9AE4B1A18030L,0x754A44C00FDFECL,0x5428A9300D4ABAL,0xEE353FCAL};
	public static final long[] CURVE_Order= {0xFDD459C6D655E1L,0x67E140D2BF941FL,0xE8CE42435B53DCL,0xB3AD58F10126DL,0xF1FD178CL};
	public static final long[] CURVE_Gx= {0xC97A2DD98F5CFFL,0xD2DCAF98B70164L,0x4749D423958C27L,0x56C139EB31183DL,0xB6B3D4C3L};
	public static final long[] CURVE_Gy= {0x115A1554062CFBL,0xC307E8E4C9E183L,0xF0F3ECEF8C2701L,0xC8B204911F9271L,0x6142E0F7L};

}

