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

// Base Bits= 28
//  nums256 Modulus
static let Modulus:[Chunk] = [0xFFFFF43,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static let R2modp:[Chunk] = [0x0,0x8900000,0x8B,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 152961
static let CURVE_B:[Chunk] = [0x25581,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x751A825,0xAB20294,0x65C6020,0x8275EA2,0xFFFE43C,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xF]
static public let CURVE_Gx:[Chunk] = [0x21AACB1,0x52EE1EB,0x4C73ABC,0x9B0903D,0xB098357,0xA04F42C,0x1297A95,0x5AAADB6,0xC9ED6B6,0xB]
static public let CURVE_Gy:[Chunk] = [0x184DE9F,0xB5B9CB2,0x10FBB80,0xC3D1153,0x35C955,0xF77E04E,0x673448B,0x3399B6A,0x8FC0F1,0xD]


#endif

#if D64

// Base Bits= 56
//  nums256 Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFF43,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static let R2modp:[Chunk] = [0x89000000000000,0x8B,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Weierstrass Curve
static let CURVE_Cof_I:Int = 1
static let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = -3
static let CURVE_B_I:Int = 152961
static let CURVE_B:[Chunk] = [0x25581,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0xAB20294751A825,0x8275EA265C6020,0xFFFFFFFFFFE43C,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x52EE1EB21AACB1,0x9B0903D4C73ABC,0xA04F42CB098357,0x5AAADB61297A95,0xBC9ED6B6]
static public let CURVE_Gy:[Chunk] = [0xB5B9CB2184DE9F,0xC3D115310FBB80,0xF77E04E035C955,0x3399B6A673448B,0xD08FC0F1]


#endif

}

