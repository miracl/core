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
//  nums384 Modulus
static let Modulus:[Chunk] = [0x1FFFFEC3,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static let R2modp:[Chunk] = [0x0,0x4448000,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x13D

//  nums384 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = -34568
static let CURVE_B:[Chunk] = [0x1FFF77BB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static public let CURVE_Order:[Chunk] = [0x1B0E61B9,0x26C0FB3,0xDF89E98,0x153A7A98,0x16881BED,0x178F75AE,0x1FFF587A,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static public let CURVE_Gx:[Chunk] = [0x98152A,0x1CE5D021,0x18711EFA,0x1DDA201E,0xC742522,0x148D9536,0x7D3CEF4,0x19BF703F,0x60225C1,0x12082F8D,0x12203288,0x2DE3038,0x17956F0B,0x3A]
static public let CURVE_Gy:[Chunk] = [0x6180716,0x3A5C763,0x1D2B4997,0xD69B77F,0x837EBCD,0x1BE890D,0xE72E482,0xEFF0FEE,0x1EB00469,0x2C267B,0x15F8CF4C,0x3371C71,0xDEE368E,0x56]


#endif

#if D64

// Base Bits= 58
//  nums384 Modulus
static let Modulus:[Chunk] = [0x3FFFFFFFFFFFEC3,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0xFFFFFFFFF]
static let R2modp:[Chunk] = [0x88900000000000,0x6,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x13D

//  nums384 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = -34568
static let CURVE_B:[Chunk] = [0x3FFFFFFFFFF77BB,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0xFFFFFFFFF]
static public let CURVE_Order:[Chunk] = [0x4D81F67B0E61B9,0x2A74F530DF89E98,0x2F1EEB5D6881BED,0x3FFFFFFFFFF587A,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0xFFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x39CBA042098152A,0x3BB4403D8711EFA,0x291B2A6CC742522,0x337EE07E7D3CEF4,0x24105F1A60225C1,0x5BC60712203288,0x757956F0B]
static public let CURVE_Gy:[Chunk] = [0x74B8EC66180716,0x1AD36EFFD2B4997,0x37D121A837EBCD,0x1DFE1FDCE72E482,0x584CF7EB00469,0x66E38E35F8CF4C,0xACDEE368E]



#endif

}

