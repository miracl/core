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


package org.apache.milagro.amcl.NIST256;


public class ROM
{

// Base Bits= 28
	public static final int[] Modulus= {0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFF,0x0,0x0,0x1000000,0x0,0xFFFFFFF,0xF};
	public static final int[] R2modp= {0x50000,0x300000,0x0,0x0,0xFFFFFFA,0xFFFFFBF,0xFFFFEFF,0xFFFAFFF,0x2FFFF,0x0};
	public static final int MConst= 0x1;

	public static final int CURVE_Cof_I= 1;
	public static final int[] CURVE_Cof= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 0;
	public static final int[] CURVE_B= {0x7D2604B,0xCE3C3E2,0x3B0F63B,0x6B0CC5,0x6BC651D,0x5576988,0x7B3EBBD,0xAA3A93E,0xAC635D8,0x5};
	public static final int[] CURVE_Order= {0xC632551,0xB9CAC2F,0x79E84F3,0xFAADA71,0xFFFBCE6,0xFFFFFFF,0xFFFFFF,0x0,0xFFFFFFF,0xF};
	public static final int[] CURVE_Gx= {0x898C296,0xA13945D,0xB33A0F4,0x7D812DE,0xF27703,0xE563A44,0x7F8BCE6,0xE12C424,0xB17D1F2,0x6};
	public static final int[] CURVE_Gy= {0x7BF51F5,0xB640683,0x15ECECB,0x33576B3,0xE162BCE,0x4A7C0F9,0xB8EE7EB,0xFE1A7F9,0xFE342E2,0x4};

}


