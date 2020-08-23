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
//  Created by Michael Scott on 12/12/2019.
//  Copyright (c) 2019 Michael Scott. All rights reserved.
//

public struct ROM{
 
#if D32
// Base Bits= 29
static let Modulus:[Chunk] = [0x1FFFFFFF,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF]
static let R2modp:[Chunk] = [0x10000000,0x0,0x4,0x8,0x0,0x0]
static let ROI:[Chunk] = [0x1FFFFFFE,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF]
static let MConst:Chunk = 0x1

static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x565FA45,0xEA6A56E,0xB3E27E0,0x1AF516CB,0x1BEFC54B,0xE4B]
static public let CURVE_Order:[Chunk] = [0xA752257,0x93D769E,0x7D323E,0x0,0x0,0x8000]
static public let CURVE_Gx:[Chunk] = [0x13CBFC82,0x61C5DC8,0x191A625A,0xAE6508C,0xB5688EF,0x254B]
static public let CURVE_Gy:[Chunk] = [0x1AC5FB32,0x11A89BB,0x17324481,0x1128FAB3,0x2855316,0x11D3]
static public let CURVE_HTPC:[Chunk] = [0x25BC0DA,0xED7D79D,0x139A9CC1,0x14C20DF5,0xBD02CD5,0xF28]
#endif

#if D64

// Base Bits= 56
static let Modulus:[Chunk] = [0xFFFFFF7FFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let R2modp:[Chunk] = [0x1000000010000,0x400000,0x0]
static let ROI:[Chunk] = [0xFFFFFF7FFFFFFE,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let MConst:Chunk = 0xFFFFFF80000001

static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xD4D4ADC565FA45,0x7A8B65ACF89F81,0x1C97BEFC54BD]
static public let CURVE_Order:[Chunk] = [0x27AED3CA752257,0x1F4C8F9,0x1000000000000]
static public let CURVE_Gx:[Chunk] = [0xC38BB913CBFC82,0x73284664698968,0x4A96B5688EF5]
static public let CURVE_Gy:[Chunk] = [0x2351377AC5FB32,0x947D59DCC91204,0x23A628553168]
static public let CURVE_HTPC:[Chunk] = [0xDAFAF3A25BC0DA,0x6106FACE6A7305,0x1E50BD02CD5A]
#endif

}

