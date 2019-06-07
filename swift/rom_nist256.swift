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
//  nist256 Curve Modulus
static let Modulus:[Chunk] = [0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFF,0x0,0x0,0x1000000,0x0,0xFFFFFFF,0xF]
static let R2modp:[Chunk] = [0x50000,0x300000,0x0,0x0,0xFFFFFFA,0xFFFFFBF,0xFFFFEFF,0xFFFAFFF,0x2FFFF,0x0]
static let MConst:Chunk = 0x1

//  nist256 Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0x7D2604B,0xCE3C3E2,0x3B0F63B,0x6B0CC5,0x6BC651D,0x5576988,0x7B3EBBD,0xAA3A93E,0xAC635D8,0x5]
static public let CURVE_Order:[Chunk] = [0xC632551,0xB9CAC2F,0x79E84F3,0xFAADA71,0xFFFBCE6,0xFFFFFFF,0xFFFFFF,0x0,0xFFFFFFF,0xF]
static public let CURVE_Gx:[Chunk] = [0x898C296,0xA13945D,0xB33A0F4,0x7D812DE,0xF27703,0xE563A44,0x7F8BCE6,0xE12C424,0xB17D1F2,0x6]
static public let CURVE_Gy:[Chunk] = [0x7BF51F5,0xB640683,0x15ECECB,0x33576B3,0xE162BCE,0x4A7C0F9,0xB8EE7EB,0xFE1A7F9,0xFE342E2,0x4]

#endif

#if D64

// Base Bits= 56
//  nist256 Curve Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFFF,0xFFFFFFFFFF,0x0,0x1000000,0xFFFFFFFF]
static let R2modp:[Chunk] = [0x3000000050000,0x0,0xFFFFFBFFFFFFFA,0xFFFAFFFFFFFEFF,0x2FFFF]
static let MConst:Chunk = 0x1

//  nist256 Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0xCE3C3E27D2604B,0x6B0CC53B0F63B,0x55769886BC651D,0xAA3A93E7B3EBBD,0x5AC635D8]
static public let CURVE_Order:[Chunk] = [0xB9CAC2FC632551,0xFAADA7179E84F3,0xFFFFFFFFFFBCE6,0xFFFFFF,0xFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0xA13945D898C296,0x7D812DEB33A0F4,0xE563A440F27703,0xE12C4247F8BCE6,0x6B17D1F2]
static public let CURVE_Gy:[Chunk] = [0xB6406837BF51F5,0x33576B315ECECB,0x4A7C0F9E162BCE,0xFE1A7F9B8EE7EB,0x4FE342E2]


#endif

}

