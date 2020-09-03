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
static let Modulus:[Chunk] = [0x1,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000]
static let R2modp:[Chunk] = [0x3FF4,0xE8E1080,0xA9C2909,0xC128269,0x89F8301,0x19607541,0x1F4C0E6F,0x3443009,0x22B644]
static let ROI:[Chunk] = [0x69D57EC,0x5E991A4,0x213EFC7,0x10FBA370,0x72930A2,0x5B22D4C,0x102FC76F,0xA21B807,0x113EFC]
static let SQRTm3:[Chunk] = [0x91F49FA,0x17E6AD31,0x19F5AE40,0x8F54DB0,0x3FFCD01,0xB5FF973,0x12C43D6C,0xE878126,0x2D8CDA]
static let MConst:Chunk = 0x1FFFFFFF

//*** rom curve parameters *****
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 5
static public let CURVE_B:[Chunk] = [0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x1,0xA032710,0xFD66E68,0x15424ED8,0x38,0x0,0x0,0x0,0x400000]
static public let CURVE_Gx:[Chunk] = [0x0,0x1657EA0,0x18A1B261,0x15424ED2,0x38,0x0,0x0,0x0,0x400000]
static public let CURVE_Gy:[Chunk] = [0x2,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_HTPC:[Chunk] = [0xD69C848,0x1C30E5ED,0xBBDA1CC,0x180EA8CE,0xE9704A,0x1172DE0D,0x1C820B1A,0x1BC54C92,0x3AC188]

#endif

#if D64

// Base  bits= 56
static let Modulus:[Chunk] = [0x2CAFD400000001,0xA127696286C984,0x38A,0x0,0x40000000]
static let R2modp:[Chunk] = [0x21CA8949BBE5DF,0x717E3D9E719200,0xB8A93694FCEE5,0xE6FCB03AA0A27E,0x2013F4C0]
static let ROI:[Chunk] = [0xBD3234869D57EC,0x7DD1B8084FBF1C,0x645A9872930A28,0xDC03C0BF1DBCB,0x113EFC51]
static let SQRTm3:[Chunk] = [0xFCD5A6291F49FA,0x7AA6D867D6B902,0xBFF2E63FFCD014,0x3C0934B10F5B16,0x2D8CDA74]
static let MConst:Chunk = 0x2CAFD3FFFFFFFF

//*** rom curve parameters *****
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 5
static public let CURVE_B:[Chunk] = [0x5,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x4064E200000001,0xA1276C3F59B9A1,0x38A,0x0,0x40000000]
static public let CURVE_Gx:[Chunk] = [0x2CAFD400000000,0xA127696286C984,0x38A,0x0,0x40000000]
static public let CURVE_Gy:[Chunk] = [0x2,0x0,0x0,0x0,0x0]
static public let CURVE_HTPC:[Chunk] = [0x861CBDAD69C848,0x754672EF68733,0xE5BC1A0E9704AC,0x2A64972082C6A2,0x3AC188DE]

#endif

}

