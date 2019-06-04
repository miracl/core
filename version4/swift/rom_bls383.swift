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

//
//  rom.swift
//
//  Created by Michael Scott on 12/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

public struct ROM{
 
#if D32


// Base Bits= 29
static let Modulus:[Chunk] = [0x5AAB0AB,0x11B8EB24,0x19214AF6,0x187E5314,0x124F47A8,0x1C00B4B0,0x1446B0C6,0x59E6CB4,0x4A0AD46,0xFF5494,0x81B6B71,0x956DD6B,0x16556956,0x2A]
static let R2modp:[Chunk] = [0x116907F4,0x405B700,0x1752AC11,0x67A9E7C,0x1941C581,0x1AEA38C4,0xB1E4D22,0xCE841AE,0xA0FC49B,0xB4B1F48,0x13852312,0x1B3FDCED,0x1FECE397,0x26]
static let MConst:Chunk = 0x73435FD
static public let Fra:[Chunk] = [0x1311DAC1,0x296B969,0x19DCF806,0x126901FC,0xD8C8A36,0x1A2572A8,0xA1A0959,0x1A47F743,0x110E4C6C,0x1608DA97,0xCE2E7F0,0x4FED178,0xACD5BF0,0x11]
static public let Frb:[Chunk] = [0x1298D5EA,0xF2231BA,0x1F4452F0,0x6155117,0x4C2BD72,0x1DB4208,0xA2CA76D,0xB567571,0x139260D9,0xAF679FC,0x1B388380,0x4580BF2,0xB880D66,0x19]

static let CURVE_A:Int = 0
static let CURVE_Cof_I:Int = 0
static public let CURVE_Cof:[Chunk] = [0x10011FF,0x400000,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
//static public let CURVE_Cof:[Chunk] = [0x15169EAB,0xA82AB0A,0xAAEFFED,0x15558001,0x555,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 15
static public let CURVE_B:[Chunk] = [0xF,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x1EBC0001,0x1904CF5F,0x834E5CE,0xBE12B42,0xB381DE0,0xE40B4C,0x270110,0x10018017,0x1002001,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Gx:[Chunk] = [0x8734573,0x623B9C8,0x1D1DC11E,0xBB7E107,0x1E3445C5,0x1D6C2578,0x10B0BE1E,0xED6103E,0x10F31D9F,0x296ED82,0x18E0D7D0,0x12F3D9C9,0x1FCBA55B,0x20]
static public let CURVE_Gy:[Chunk] = [0x3F224,0x968B2F4,0x1FE63F48,0xFA93D90,0x14D2DDE5,0x54A56F5,0x12441D4C,0x18CD76C8,0x199D0DAD,0xE18E236,0x92BA73,0x99F6600,0x8F16727,0x3]

static public let CURVE_Bnx:[Chunk] = [0x1001200,0x400000,0x40,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Cru:[Chunk] = [0xEAAC2A9,0x61B3A81,0x17D974B7,0xBED0345,0xA341BC2,0x17A51A6F,0x5738948,0x69B7BAE,0x14605445,0x374A43,0x8116AD1,0x956DD69,0x16556956,0x2A]
static public let CURVE_Pxa:[Chunk] = [0xD7F2D86,0x1E59DB1,0x17474F85,0x1FB56CF2,0x572EE81,0xE487AB1,0x96F51FC,0x190A5AAE,0x6432501,0x13E58F3A,0x101E6425,0xFD807D1,0x34D2240,0x3]
static public let CURVE_Pxb:[Chunk] = [0x452DE15,0x1ECF20F6,0x1FF9837B,0x95651AA,0xD5D75B5,0x5D44749,0x12277F66,0x1DB3A0B9,0x1D24F498,0x19441B0E,0x1CDE9DC5,0x2C975,0xD78006,0x18]
static public let CURVE_Pya:[Chunk] = [0x1408CB41,0x34785DC,0x3586597,0x13DBC9E4,0x1A2E75B4,0x1D65489,0xCF9A25E,0x1ACE7933,0x1B6E990E,0x19FF31A3,0x12527615,0x1A44A68F,0x1792CF93,0x19]
static public let CURVE_Pyb:[Chunk] = [0x1F479093,0x16C2321B,0x1889218E,0x87961BC,0x1BC98B01,0x197A24FB,0xA3DEBC2,0x88D67DF,0x1CE0D,0x1E8AD3D7,0x93B9EE9,0x59B18D6,0xE5247DD,0x10]
static let CURVE_W:[[Chunk]] = [[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]]
static let CURVE_SB:[[[Chunk]]] = [[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]]]
static let CURVE_WB:[[Chunk]] = [[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]]
static let CURVE_BB:[[[Chunk]]] = [[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]]]    

#endif

#if D64

// Base Bits= 58
static let Modulus:[Chunk] = [0x2371D6485AAB0AB,0x30FCA6299214AF6,0x3801696124F47A8,0xB3CD969446B0C6,0x1FEA9284A0AD46,0x12ADBAD681B6B71,0x556556956]
static let R2modp:[Chunk] = [0x80B6E0116907F4,0xCF53CF9752AC11,0x35D47189941C581,0x19D0835CB1E4D22,0x16963E90A0FC49B,0x367FB9DB3852312,0x4DFECE397]
static let MConst:Chunk = 0x1BC0571073435FD
static public let Fra:[Chunk] = [0x52D72D3311DAC1,0x24D203F99DCF806,0x344AE550D8C8A36,0x348FEE86A1A0959,0x2C11B52F10E4C6C,0x9FDA2F0CE2E7F0,0x22ACD5BF0]
static public let Frb:[Chunk] = [0x1E446375298D5EA,0xC2AA22FF4452F0,0x3B684104C2BD72,0x16ACEAE2A2CA76D,0x15ECF3F939260D9,0x8B017E5B388380,0x32B880D66]

//*** rom curve parameters *****
// Base Bits= 58

static let CURVE_A:Int = 0
static let CURVE_Cof_I:Int = 0
static public let CURVE_Cof:[Chunk] = [0x80000010011FF,0x40,0x0,0x0,0x0,0x0,0x0]
//static public let CURVE_Cof:[Chunk] = [0x150556155169EAB,0x2AAB0002AAEFFED,0x555,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 15
static public let CURVE_B:[Chunk] = [0xF,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x32099EBFEBC0001,0x17C25684834E5CE,0x1C81698B381DE0,0x2003002E0270110,0x1002001,0x0,0x0]
static public let CURVE_Gx:[Chunk] = [0xC4773908734573,0x176FC20FD1DC11E,0x3AD84AF1E3445C5,0x1DAC207D0B0BE1E,0x52DDB050F31D9F,0x25E7B3938E0D7D0,0x41FCBA55B]
static public let CURVE_Gy:[Chunk] = [0x12D165E8003F224,0x1F527B21FE63F48,0xA94ADEB4D2DDE5,0x319AED912441D4C,0x1C31C46D99D0DAD,0x133ECC00092BA73,0x68F16727]

static public let CURVE_Bnx:[Chunk] = [0x8000001001200,0x40,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Cru:[Chunk] = [0xC367502EAAC2A9,0x17DA068B7D974B7,0x2F4A34DEA341BC2,0xD36F75C5738948,0x6E94874605445,0x12ADBAD28116AD1,0x556556956]
static public let CURVE_Pxa:[Chunk] = [0x3CB3B62D7F2D86,0x3F6AD9E57474F85,0x1C90F562572EE81,0x3214B55C96F51FC,0x27CB1E746432501,0x1FB00FA301E6425,0x634D2240]
static public let CURVE_Pxb:[Chunk] = [0x3D9E41EC452DE15,0x12ACA355FF9837B,0xBA88E92D5D75B5,0x3B6741732277F66,0x3288361DD24F498,0x592EBCDE9DC5,0x300D78006]
static public let CURVE_Pya:[Chunk] = [0x68F0BB9408CB41,0x27B793C83586597,0x3ACA913A2E75B4,0x359CF266CF9A25E,0x33FE6347B6E990E,0x34894D1F2527615,0x33792CF93]
static public let CURVE_Pyb:[Chunk] = [0x2D846437F479093,0x10F2C379889218E,0x32F449F7BC98B01,0x111ACFBEA3DEBC2,0x3D15A7AE001CE0D,0xB3631AC93B9EE9,0x20E5247DD]
static let CURVE_W:[[Chunk]] = [[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]]
static let CURVE_SB:[[[Chunk]]] = [[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]]]
static let CURVE_WB:[[Chunk]] = [[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]]
static let CURVE_BB:[[[Chunk]]] = [[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]],[[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0],[0x0,0x0,0x0,0x0,0x0,0x0,0x0]]]

#endif


}

