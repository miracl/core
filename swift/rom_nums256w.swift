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

// Base Bits= 28
//  nums256 Modulus
static let Modulus:[Chunk] = [0xFFFFF43,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static let R2modp:[Chunk] = [0x0,0x8900000,0x8B,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 152961
static let CURVE_B:[Chunk] = [0x25581,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x751A825,0xAB20294,0x65C6020,0x8275EA2,0xFFFE43C,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static public let CURVE_Gx:[Chunk] = [0x21AACB1,0x52EE1EB,0x4C73ABC,0x9B0903D,0xB098357,0xA04F42C,0x1297A95,0x5AAADB6,0xC9ED6B6,0xB]
static public let CURVE_Gy:[Chunk] = [0x184DE9F,0xB5B9CB2,0x10FBB80,0xC3D1153,0x35C955,0xF77E04E,0x673448B,0x3399B6A,0x8FC0F1,0xD]


#endif

#if D64

// Base Bits= 56
//  nums256 Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFF43,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static let R2modp:[Chunk] = [0x89000000000000,0x8B,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 152961
static let CURVE_B:[Chunk] = [0x25581,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0xAB20294751A825,0x8275EA265C6020,0xFFFFFFFFFFE43C,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x52EE1EB21AACB1,0x9B0903D4C73ABC,0xA04F42CB098357,0x5AAADB61297A95,0xBC9ED6B6]
static public let CURVE_Gy:[Chunk] = [0xB5B9CB2184DE9F,0xC3D115310FBB80,0xF77E04E035C955,0x3399B6A673448B,0xD08FC0F1]


#endif

}

