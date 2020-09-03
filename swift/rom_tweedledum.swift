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
//  Created by Michael Scott on 12/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

public struct ROM{
 
#if D32


// Base  bits= 29
static let Modulus:[Chunk] = [0x1,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000]
static let R2modp:[Chunk] = [0x3FF4,0x1B622B40,0x1166FB28,0x5BEEDDF,0x16646668,0x321298C,0xDD7B28E,0x3E66507,0x22B644]
static let ROI:[Chunk] = [0x188837CE,0x1EF4F98D,0x1AA67594,0xDD1E7E7,0x2D57FF4,0x197557C6,0xBF3D893,0x2F121DC,0x2AE451]
static let SQRTm3:[Chunk] = [0x17DE6C70,0x10786BD5,0x1DC95B25,0xD55C069,0x184CB128,0x16EB9107,0x1B5B20A1,0x95285A0,0x15EF7D]
static let MConst:Chunk = 0x1FFFFFFF

//*** rom curve parameters *****
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 5
static public let CURVE_B:[Chunk] = [0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x1,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000]
static public let CURVE_Gx:[Chunk] = [0x0,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000]
static public let CURVE_Gy:[Chunk] = [0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_HTPC:[Chunk] = [0x1381795,0x16B3AA9A,0xF3EF0D9,0x15DD7A0,0xBAA53E1,0xC45B655,0xFD3BC53,0x1E66E041,0x3AB46C]

#endif

#if D64

// Base  bits= 56
static let Modulus:[Chunk] = [0x4064E200000001,0xA1276C3F59B9A1,0x38A,0x0,0x40000000]
static let R2modp:[Chunk] = [0xFC85D749BBE0CD,0x64ED6614FCA69B,0x9925C8F6AC3D6D,0x28E19094C65991,0xA0EDD7B]
static let ROI:[Chunk] = [0xDE9F31B88837CE,0xE8F3F3EA99D653,0xEAAF8C2D57FF46,0x890EE2FCF624F2,0x2AE45117]
static let SQRTm3:[Chunk] = [0xF0D7AB7DE6C70,0xAAE034F7256C96,0xD7220F84CB1286,0x942D06D6C8286D,0x15EF7D4A]
static let MConst:Chunk = 0x4064E1FFFFFFFF

//*** rom curve parameters *****
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 5
static public let CURVE_B:[Chunk] = [0x5,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x2CAFD400000001,0xA127696286C984,0x38A,0x0,0x40000000]
static public let CURVE_Gx:[Chunk] = [0x4064E200000000,0xA1276C3F59B9A1,0x38A,0x0,0x40000000]
static public let CURVE_Gy:[Chunk] = [0x2,0x0,0x0,0x0,0x0]
static public let CURVE_HTPC:[Chunk] = [0xD6755341381795,0xAEEBD03CFBC366,0x8B6CAABAA53E10,0x37020BF4EF14D8,0x3AB46CF3]

#endif

}

