/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

//
//  rom.swift
//
//  Created by Michael Scott on 15/04/2019.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

public struct ROM{
 
#if D32
// Base Bits= 28
static let Modulus:[Chunk] = [0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF]
static let ROI:[Chunk] = [0xFFFFFFE,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF]
static let R2modp:[Chunk] = [0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0]
static let MConst:Chunk = 0x1

//*** rom curve parameters *****
// Base Bits= 28

static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xD940E93,0xBCBD414,0xB8F92DD,0x89F515A,0x9A7F397,0x4BCF650,0x44D5A9E,0x9D9F5E3,0x8E9FA9E,0x2]
static public let CURVE_Order:[Chunk] = [0x9D54123,0xBBF4093,0x6052B53,0xDF6B21C,0xFFF7203,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF]
static public let CURVE_Gx:[Chunk] = [0x34C74C7,0x5A45893,0x60BE171,0xBBFF26,0x9948FE3,0x466A39C,0x95F9904,0x1F19811,0x2C4AE2C,0x3]
static public let CURVE_Gy:[Chunk] = [0x139F0A0,0xDF32E52,0xA474002,0x877CC62,0x153D0A9,0xE36B692,0xC59BDCE,0xF4F6779,0xC3736A2,0xB]
static public let CURVE_HTPC:[Chunk] = [0xFFFFFFF,0xFFFFFFF,0x55555FF,0xFFFF555,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xAAAAAA9,0xA]
#endif

#if D64

// Base Bits= 56
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFFF,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE]
static let ROI:[Chunk] = [0xFFFFFFFFFFFFFE,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE]
static let R2modp:[Chunk] = [0x3000000040000,0xFFFFFFFE000000,0x300000002,0x10000000100,0x60000]
static let MConst:Chunk = 0x1

//*** rom curve parameters *****
// Base Bits= 56

static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xBCBD414D940E93,0x89F515AB8F92DD,0x4BCF6509A7F397,0x9D9F5E344D5A9E,0x28E9FA9E]
static public let CURVE_Order:[Chunk] = [0xBBF40939D54123,0xDF6B21C6052B53,0xFFFFFFFFFF7203,0xFFFFFFFFFFFFFF,0xFFFFFFFE]
static public let CURVE_Gx:[Chunk] = [0x5A4589334C74C7,0xBBFF2660BE171,0x466A39C9948FE3,0x1F1981195F9904,0x32C4AE2C]
static public let CURVE_Gy:[Chunk] = [0xDF32E52139F0A0,0x877CC62A474002,0xE36B692153D0A9,0xF4F6779C59BDCE,0xBC3736A2]
static public let CURVE_HTPC:[Chunk] = [0xFFFFFFFFFFFFFF,0xFFFF55555555FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xAAAAAAA9]
#endif

}

