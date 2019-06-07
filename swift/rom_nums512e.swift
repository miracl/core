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
//  nums512 Modulus
static let Modulus:[Chunk] = [0x1FFFFDC7,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static let R2modp:[Chunk] = [0xB100000,0x278,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x239

//  nums512 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -78296
static public let CURVE_B:[Chunk] = [0x1FFECBEF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static public let CURVE_Order:[Chunk] = [0x1BEED46D,0x1A3467A8,0x1BFB3FD9,0xC0AF0DB,0x86F52A4,0xC64B85B,0x6EA78FF,0xDA5F9F2,0x1FB4F063,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFF]
static public let CURVE_Gx:[Chunk] = [0x19EC57FE,0xDCD594C,0x113C0571,0xA4A84F9,0x104AD0FE,0x4C92B44,0xC3DE2F7,0x9DDC8CE,0x74621C1,0x1139DC0A,0x9E85FAF,0x1B894704,0x1D1E79F4,0x9E29997,0x32DE223,0x16D38F43,0x116D128D,0x6FC71]
static public let CURVE_Gy:[Chunk] = [0x1E2F5E1,0x136EF606,0x1C7407CC,0xDA71537,0xC1FD026,0x3431576,0x15898068,0x1E5D32C6,0x120CA53,0xC84F41A,0xA4ADAE5,0x104B3A45,0x76F726D,0x1512B772,0x3D5DEA0,0x194E3316,0x1FF39D49,0x3684D]


//  nums384 Edwards Curve

#endif

#if D64

// Base Bits= 60
//  nums512 Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFDC7,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFF]
static let R2modp:[Chunk] = [0x100000000000000,0x4F0B,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x239

//  nums512 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -78296
static let CURVE_B:[Chunk] = [0xFFFFFFFFFFECBEF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFF]
static public let CURVE_Order:[Chunk] = [0x7468CF51BEED46D,0x4605786DEFECFF6,0xFD8C970B686F52A,0x636D2FCF91BA9E3,0xFFFFFFFFFFFB4F0,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFFF,0x3FFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x5B9AB2999EC57FE,0xE525427CC4F015C,0xDC992568904AD0F,0xC14EEE46730F78B,0xEBE273B81474621,0x9F4DC4A38227A17,0x888D3C5332FD1E7,0x128DB69C7A18CB7,0xDF8E316D]
static public let CURVE_Gy:[Chunk] = [0x26DDEC0C1E2F5E1,0x66D38A9BF1D01F3,0xA06862AECC1FD02,0x53F2E9963562601,0xB95909E834120CA,0x26D8259D22A92B6,0x7A82A256EE476F7,0x9D49CA7198B0F57,0x6D09BFF3]



#endif

}

