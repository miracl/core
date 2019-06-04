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

// Base Bits= 28
public static final int[] Modulus= {0xFFFFC2F,0xFFFFFEF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
public static final int[] R2modp= {0x0,0xA100000,0x2000E90,0x7A,0x1,0x0,0x0,0x0,0x0,0x0};
public static final int MConst= 0x2253531;

public static final int CURVE_Cof_I= 1;
public static final int[] CURVE_Cof= {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
public static final int CURVE_A= 0;
public static final int CURVE_B_I= 7;
public static final int[] CURVE_B= {0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0};
public static final int[] CURVE_Order= {0x364141,0xD25E8CD,0x8A03BBF,0xDCE6AF4,0xFFEBAAE,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF};
public static final int[] CURVE_Gx= {0x6F81798,0xF2815B1,0xE28D959,0xFCDB2DC,0xB07029B,0x95CE870,0xC55A062,0xF9DCBBA,0x9BE667E,0x7};
public static final int[] CURVE_Gy= {0xB10D4B8,0x47D08FF,0x554199C,0xB448A68,0x8A8FD17,0xFC0E110,0x55DA4FB,0x26A3C46,0x83ADA77,0x4};

}


