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

static let CURVE_A:Int = -3
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x565FA45,0xEA6A56E,0xB3E27E0,0x1AF516CB,0x1BEFC54B,0xE4B]
static public let CURVE_Order:[Chunk] = [0xA752257,0x93D769E,0x7D323E,0x0,0x0,0x8000]
static public let CURVE_Gx:[Chunk] = [0x13CBFC82,0x61C5DC8,0x191A625A,0xAE6508C,0xB5688EF,0x254B]
static public let CURVE_Gy:[Chunk] = [0x1AC5FB32,0x11A89BB,0x17324481,0x1128FAB3,0x2855316,0x11D3]

#endif

#if D64

// Base Bits= 56
static let Modulus:[Chunk] = [0xFFFFFF7FFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let R2modp:[Chunk] = [0x1000000010000,0x400000,0x0]
static let ROI:[Chunk] = [0xFFFFFF7FFFFFFE,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF]
static let MConst:Chunk = 0xFFFFFF80000001

static let CURVE_A:Int = -3
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xD4D4ADC565FA45,0x7A8B65ACF89F81,0x1C97BEFC54BD]
static public let CURVE_Order:[Chunk] = [0x27AED3CA752257,0x1F4C8F9,0x1000000000000]
static public let CURVE_Gx:[Chunk] = [0xC38BB913CBFC82,0x73284664698968,0x4A96B5688EF5]
static public let CURVE_Gy:[Chunk] = [0x2351377AC5FB32,0x947D59DCC91204,0x23A628553168]

#endif

}

