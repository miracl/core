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

// Base Bits= 29
static let Modulus:[Chunk] = [0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF]
static let R2modp:[Chunk] = [0x190000,0x0,0x0,0x0,0x0,0x0]
static let ROI:[Chunk] = [0x1FFFFFFA,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFF]
static let MConst:Chunk = 0x5

//*** rom curve parameters *****
// Base Bits= 29

static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 5766
static public let CURVE_B:[Chunk] = [0x1686,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x1DBA8B27,0x7F854C,0x1F57BC06,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static public let CURVE_Gx:[Chunk] = [0x19D52398,0x138DCEDF,0x183D99B1,0x1340C31D,0x1A505B80,0xA64A6]
static public let CURVE_Gy:[Chunk] = [0x4920345,0x3843D92,0x758B70B,0x77F8EE7,0x149BC0A1,0x14A0A2]
static public let CURVE_HTPC:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0]
#endif

#if D64

// Base Bits= 60
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFFFB,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF]
static let R2modp:[Chunk] = [0x190000000,0x0,0x0]
static let ROI:[Chunk] = [0xFFFFFFFFFFFFFFA,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF]
static let MConst:Chunk = 0x5

//*** rom curve parameters *****
// Base Bits= 60

static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0]
static let CURVE_B_I:Int = 5766
static public let CURVE_B:[Chunk] = [0x1686,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x80FF0A99DBA8B27,0xFFFFFFFFFD5EF01,0xFFFFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x671B9DBF9D52398,0x9A0618EE0F666C,0x14C94DA505B8]
static public let CURVE_Gy:[Chunk] = [0xC7087B244920345,0x13BFC7739D62DC2,0x29414549BC0A]
static public let CURVE_HTPC:[Chunk] = [0x1,0x0,0x0]
#endif

}

