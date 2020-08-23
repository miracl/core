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


// Base Bits= 28
// Million Dollar Curve Modulus
static public let Modulus:[Chunk] = [0x779EC13,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF]
static let R2modp:[Chunk] = [0xDA5D1FE,0xDE5B77F,0x2985975,0x3228385,0x95FA423,0xA24A9FC,0xA869FAE,0x6853D0E,0xEC4B765,0x3]
static let ROI:[Chunk] = [0x779EC12,0x3710405,0x7AF9EC1,0x9037EF0,0xD4F086A,0xFDD7A5F,0x4532F92,0xD456AFB,0x13B68B9,0xF]
static let MConst:Chunk = 0xAE541E5

static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0x300CF89,0x9309250,0x1F041EE,0x70F63F2,0x9625F12,0xCCFB0CB,0xA7CDFBF,0x1965B68,0x7130452,0x5]
static public let CURVE_Order:[Chunk] = [0x1B0F7FB,0x8552F42,0x974BB58,0x8975FAF,0xF534FB3,0xBF75E97,0xD14CBE4,0x7515ABE,0xC4EDA2E,0x3]
static public let CURVE_Gx:[Chunk] = [0xE68363B,0x2C25BFE,0x713E223,0x2ABB8D2,0xF6350D7,0x1E03CBC,0x3D85B42,0x7F903B8,0x681886A,0xB]
static public let CURVE_Gy:[Chunk] = [0xD2864B5,0x73C355B,0x93A3A7E,0xBC3D81A,0xA421DA8,0xCF6563D,0x359814D,0xB59C0B0,0xA6734E1,0xC]
static public let CURVE_HTPC:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]

#endif

#if D64

// Base Bits= 56
// Million Dollar Curve Modulus
static public let Modulus:[Chunk] = [0x3710405779EC13,0x9037EF07AF9EC1,0xFDD7A5FD4F086A,0xD456AFB4532F92,0xF13B68B9]
static let R2modp:[Chunk] = [0xDE5B77FDA5D1FE,0x32283852985975,0xA24A9FC95FA423,0x6853D0EA869FAE,0x3EC4B765]
static let ROI:[Chunk] = [0x3710405779EC12,0x9037EF07AF9EC1,0xFDD7A5FD4F086A,0xD456AFB4532F92,0xF13B68B9]
static let MConst:Chunk = 0xF10FDDAAE541E5

//*** rom curve parameters *****
// Base Bits= 56

static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static let CURVE_B:[Chunk] = [0x9309250300CF89,0x70F63F21F041EE,0xCCFB0CB9625F12,0x1965B68A7CDFBF,0x57130452]
static public let CURVE_Order:[Chunk] = [0x8552F421B0F7FB,0x8975FAF974BB58,0xBF75E97F534FB3,0x7515ABED14CBE4,0x3C4EDA2E]
static public let CURVE_Gx:[Chunk] = [0x2C25BFEE68363B,0x2ABB8D2713E223,0x1E03CBCF6350D7,0x7F903B83D85B42,0xB681886A]
static public let CURVE_Gy:[Chunk] = [0x73C355BD2864B5,0xBC3D81A93A3A7E,0xCF6563DA421DA8,0xB59C0B0359814D,0xCA6734E1]
static public let CURVE_HTPC:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
#endif

}

