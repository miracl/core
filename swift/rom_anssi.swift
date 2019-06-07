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
//  anssi Curve Modulus
static let Modulus:[Chunk] = [0x86E9C03,0xFCF353D,0x8CA6DE8,0xADBCABC,0x35B3961,0xE8CE424,0xF10126D,0xB3AD58,0x1FD178C,0xF]
static let R2modp:[Chunk] = [0x288CC9C,0x18D2374,0x646BD2B,0x4929E67,0xD6F7F2D,0x220E6C1,0xABCE02E,0x751B1FD,0x7401B78,0xE]
static let MConst:Chunk = 0x64E1155

//  anssi Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0xB7BB73F,0x75ED967,0x1A18030,0xC9AE4B,0xFDFEC,0x754A44C,0xD4ABA,0x5428A93,0xE353FCA,0xE]
static public let CURVE_Order:[Chunk] = [0x6D655E1,0xFDD459C,0x2BF941F,0x67E140D,0x35B53DC,0xE8CE424,0xF10126D,0xB3AD58,0x1FD178C,0xF]
static public let CURVE_Gx:[Chunk] = [0x98F5CFF,0xC97A2DD,0x8B70164,0xD2DCAF9,0x3958C27,0x4749D42,0xB31183D,0x56C139E,0x6B3D4C3,0xB]
static public let CURVE_Gy:[Chunk] = [0x4062CFB,0x115A155,0x4C9E183,0xC307E8E,0xF8C2701,0xF0F3ECE,0x11F9271,0xC8B2049,0x142E0F7,0x6]


#endif

#if D64

// Base Bits= 56
//  anssi Curve Modulus
static let Modulus:[Chunk] = [0xFCF353D86E9C03,0xADBCABC8CA6DE8,0xE8CE42435B3961,0xB3AD58F10126D,0xF1FD178C]
static let R2modp:[Chunk] = [0x18D2374288CC9C,0x4929E67646BD2B,0x220E6C1D6F7F2D,0x751B1FDABCE02E,0xE7401B78]
static let MConst:Chunk = 0x97483A164E1155

//  anssi Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0x75ED967B7BB73F,0xC9AE4B1A18030,0x754A44C00FDFEC,0x5428A9300D4ABA,0xEE353FCA]
static public let CURVE_Order:[Chunk] = [0xFDD459C6D655E1,0x67E140D2BF941F,0xE8CE42435B53DC,0xB3AD58F10126D,0xF1FD178C]
static public let CURVE_Gx:[Chunk] = [0xC97A2DD98F5CFF,0xD2DCAF98B70164,0x4749D423958C27,0x56C139EB31183D,0xB6B3D4C3]
static public let CURVE_Gy:[Chunk] = [0x115A1554062CFB,0xC307E8E4C9E183,0xF0F3ECEF8C2701,0xC8B204911F9271,0x6142E0F7]


#endif

}

