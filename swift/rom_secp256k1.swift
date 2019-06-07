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
//  secp256k1 Curve Modulus
static let Modulus:[Chunk] = [0xFFFFC2F,0xFFFFFEF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static let R2modp:[Chunk] = [0x0,0xA100000,0x2000E90,0x7A,0x1,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x2253531

//  secp256k1 Curve
static let CURVE_A:Int = 0
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 7
static public let CURVE_B:[Chunk] = [0x7,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x364141,0xD25E8CD,0x8A03BBF,0xDCE6AF4,0xFFEBAAE,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static public let CURVE_Gx:[Chunk] = [0x6F81798,0xF2815B1,0xE28D959,0xFCDB2DC,0xB07029B,0x95CE870,0xC55A062,0xF9DCBBA,0x9BE667E,0x7]
static public let CURVE_Gy:[Chunk] = [0xB10D4B8,0x47D08FF,0x554199C,0xB448A68,0x8A8FD17,0xFC0E110,0x55DA4FB,0x26A3C46,0x83ADA77,0x4]

#endif

#if D64

// Base Bits= 56
//  secp256k1 Curve Modulus
static let Modulus:[Chunk] = [0xFFFFFEFFFFFC2F,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static let R2modp:[Chunk] = [0xA1000000000000,0x7A2000E90,0x1,0x0,0x0]
static let MConst:Chunk = 0x38091DD2253531

//  secp256k1 Curve
static let CURVE_A:Int = 0
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 7
static public let CURVE_B:[Chunk] = [0x7,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0xD25E8CD0364141,0xDCE6AF48A03BBF,0xFFFFFFFFFEBAAE,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0xF2815B16F81798,0xFCDB2DCE28D959,0x95CE870B07029B,0xF9DCBBAC55A062,0x79BE667E]
static public let CURVE_Gy:[Chunk] = [0x47D08FFB10D4B8,0xB448A68554199C,0xFC0E1108A8FD17,0x26A3C4655DA4FB,0x483ADA77]


#endif

}

