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

//  jubjub Curve Modulus
// Base Bits= 29
static let Modulus:[Chunk] = [0x1,0x1FFFFFF8,0x1F96FFBF,0x1B4805FF,0x1D80553B,0xC0404D0,0x1520CCE7,0xA6533AF,0x73EDA7]
static let R2modp:[Chunk] = [0xA71B3C0,0x1D32207E,0x1663D999,0x1C5ABC93,0x3B58C44,0xBE37438,0x829F771,0x1660139E,0x27FD91]
static let ROI:[Chunk] = [0xB912F1F,0x1BC47A80,0xCF82506,0x49FE4E1,0xD6C0C40,0x16E468B4,0xDBC3F55,0x13CB682D,0x212D7]
static let MConst:Chunk = 0x1FFFFFFF

//*** rom curve parameters *****
// Base Bits= 29

static let CURVE_Cof_I:Int = 8
static public let CURVE_Cof:[Chunk] = [0x8,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x16343EB1,0x832FEB6,0x15E74980,0x1AFEDA6E,0x17FD4292,0x903F35E,0xD23D7F6,0x1CE97F45,0x2A9318]
static public let CURVE_Order:[Chunk] = [0x16F72CB7,0x4B872F6,0x120420B4,0x10412799,0x3B00A66,0x1D80809A,0x1EA4199C,0x1D4CA675,0xE7DB4]
static public let CURVE_Gx:[Chunk] = [0x14F976C4,0x1A7678D3,0x1CE7B79F,0x18A8D7E5,0x4882000,0x1A6F801C,0xE329892,0x55F1DFE,0x518397]
static public let CURVE_Gy:[Chunk] = [0x1349702E,0x35D6B84,0x9A0CEC5,0x190E0FFF,0x1C308096,0x62D5ECE,0x10B27A3F,0x8E5945F,0x3B43F8]
static public let CURVE_HTPC:[Chunk] = [0x47BBBB6,0x83D77CF,0x1D4C82A6,0x1BFEA2A2,0xD546E25,0x1765A39E,0x7500F8F,0x168566F6,0x62FD68]
#endif

#if D64

//  jubjub Curve Modulus
// Base Bits= 56
static let Modulus:[Chunk] = [0xFFFFFF00000001,0xA402FFFE5BFEFF,0x809A1D80553BD,0x299D7D483339D8,0x73EDA753]
static let R2modp:[Chunk] = [0x3B3440EC31BBA9,0x8929657E045FB0,0x2D645CF57C6E1A,0xEA6A1C5012ECF5,0x3C7B9D12]
static let ROI:[Chunk] = [0x788F500B912F1F,0x4FF270B3E0941B,0xC8D168D6C0C402,0x5B416B6F0FD56D,0x212D79E]
static let MConst:Chunk = 0xFFFFFEFFFFFFFF

//*** rom curve parameters *****
// Base Bits= 56

static let CURVE_Cof_I:Int = 8
static public let CURVE_Cof:[Chunk] = [0x8,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x65FD6D6343EB1,0x7F6D37579D2601,0x7E6BD7FD4292D,0x4BFA2B48F5FD92,0x2A9318E7]
static public let CURVE_Order:[Chunk] = [0x970E5ED6F72CB7,0x2093CCC81082D0,0x101343B00A668,0x6533AFA906673B,0xE7DB4EA]
static public let CURVE_Gx:[Chunk] = [0x4ECF1A74F976C4,0x546BF2F39EDE7F,0xDF00384882000C,0xF8EFF38CA624B4,0x5183972A]
static public let CURVE_Gy:[Chunk] = [0x6BAD709349702E,0x8707FFA6833B14,0x5ABD9DC308096C,0x2CA2FC2C9E8FCC,0x3B43F847]
static public let CURVE_HTPC:[Chunk] = [0x7AEF9E47BBBB6,0xFF515175320A99,0xCB473CD546E25D,0x2B37B1D403E3EE,0x62FD68B4]
#endif

}

