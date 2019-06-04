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
// C25519 Curve Modulus
	static let Modulus:[Chunk] = [0x1FFFFFED,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFFF]
	static let R2modp:[Chunk] = [0x169000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
	static let MConst:Chunk = 0x13;

// ED25519 Curve
	static let CURVE_Cof_I:Int = 8
	static let CURVE_Cof:[Chunk] = [0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
	static let CURVE_A:Int = -1
	static let CURVE_B_I:Int = 0
	static let CURVE_B:[Chunk] = [0x135978A3,0xF5A6E50,0x10762ADD,0x149A82,0x1E898007,0x3CBBBC,0x19CE331D,0x1DC56DFF,0x52036C]
	static public let CURVE_Order:[Chunk] = [0x1CF5D3ED,0x9318D2,0x1DE73596,0x1DF3BD45,0x14D,0x0,0x0,0x0,0x100000]
	static public let CURVE_Gx:[Chunk] = [0xF25D51A,0xAB16B04,0x969ECB2,0x198EC12A,0xDC5C692,0x1118FEEB,0xFFB0293,0x1A79ADCA,0x216936]
	static public let CURVE_Gy:[Chunk] = [0x6666658,0x13333333,0x19999999,0xCCCCCCC,0x6666666,0x13333333,0x19999999,0xCCCCCCC,0x666666]

 
#endif

#if D64

// Base Bits= 56
// C25519 Curve Modulus
	static let Modulus:[Chunk] = [0xFFFFFFFFFFFFED,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFFF]
	static let R2modp:[Chunk] = [0xA4000000000000,0x5,0x0,0x0,0x0]
	static let MConst:Chunk = 0x13

// ED25519 Curve
	static let CURVE_Cof_I:Int = 8
	static let CURVE_Cof:[Chunk] = [0x8,0x0,0x0,0x0,0x0]
	static let CURVE_A:Int = -1
	static let CURVE_B_I:Int = 0
	static let CURVE_B:[Chunk] = [0xEB4DCA135978A3,0xA4D4141D8AB75,0x797779E8980070,0x2B6FFE738CC740,0x52036CEE]
	static public let CURVE_Order:[Chunk] = [0x12631A5CF5D3ED,0xF9DEA2F79CD658,0x14DE,0x0,0x10000000]
	static public let CURVE_Gx:[Chunk] = [0x562D608F25D51A,0xC7609525A7B2C9,0x31FDD6DC5C692C,0xCD6E53FEC0A4E2,0x216936D3]
	static public let CURVE_Gy:[Chunk] = [0x66666666666658,0x66666666666666,0x66666666666666,0x66666666666666,0x66666666]

#endif

}

