/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing     
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.     
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
//  nums384 Modulus
static let Modulus:[Chunk] = [0x1FFFFEC3,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static let ROI:[Chunk] = [0x1FFFFEC2,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static let R2modp:[Chunk] = [0x0,0x4448000,0x6,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x13D

//  nums384 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -11556
static let CURVE_B:[Chunk] = [0x1FFFD19F,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7F]
static public let CURVE_Order:[Chunk] = [0x6A3897D,0x5CEE627,0xD721E48,0x8AAB556,0x1E1CF61E,0xD0E5A35,0x1FFF891C,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1F]
static public let CURVE_Gx:[Chunk] = [0xC206BDE,0x6AA0723,0x116504D4,0x52562CA,0x163406FF,0x1FD47998,0x10015D8F,0x8DCB7C9,0x15B30BF4,0x14D72AED,0x102DA884,0xB524CD9,0x1B111FB4,0x30]
static public let CURVE_Gy:[Chunk] = [0x10729392,0xC681F0F,0x1B123727,0x561F28D,0x1964B007,0xC7BFB22,0x1D5A0C3E,0xE9E284B,0x1716AD82,0x11D886E,0x1CE2C69,0x134DDD61,0x983E67B,0x41]


#endif

#if D64

// Base Bits= 56
//  nums384 Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFEC3,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let ROI:[Chunk] = [0xFFFFFFFFFFFEC2,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let R2modp:[Chunk] = [0x188890000,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0x13D

//  nums384 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -11556
static let CURVE_B:[Chunk] = [0xFFFFFFFFFFD19F,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static public let CURVE_Order:[Chunk] = [0xB9DCC4E6A3897D,0x555AAB35C87920,0x1CB46BE1CF61E4,0xFFFFFFFFE2471A,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x3FFFFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0xD540E46C206BDE,0x92B16545941350,0xA8F33163406FF2,0xE5BE4C005763FF,0xE55DB5B30BF446,0x266CC0B6A2129A,0x61B111FB45A9]
static public let CURVE_Gy:[Chunk] = [0x8D03E1F0729392,0xB0F946EC48DC9D,0xF7F645964B0072,0xF1425F56830F98,0xB10DD716AD8274,0xEEB08738B1A423,0x82983E67B9A6]


#endif

}

