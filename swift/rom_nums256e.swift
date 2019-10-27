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
//  nums256 Modulus
static let Modulus:[Chunk] = [0x1FFFFF43,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFFFFFF]
static let ROI:[Chunk] = [0x1FFFFF42,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFFFFFF]
static let R2modp:[Chunk] = [0x22E2400,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -15342
static let CURVE_B:[Chunk] = [0x1FFFC355,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0xFFFFFF]
static public let CURVE_Order:[Chunk] = [0xEDD4AF5,0x123D8C87,0x1650E6C6,0xAB54A5E,0x419,0x0,0x0,0x0,0x400000]
static public let CURVE_Gx:[Chunk] = [0xEED13DA,0x6F60481,0x20D61A8,0x13141DC6,0x9BD60C3,0x1EAFB490,0xDF73478,0x1F6D5D44,0x8A7514]
static public let CURVE_Gy:[Chunk] = [0x198A89E6,0x1D30B73B,0x15BB4CB,0x1EC3B021,0x18010715,0x12ECD325,0x171F3A59,0x13FB3B24,0x44D53E]


#endif

#if D64

// Base Bits= 56
//  nums256 Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFF43,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static let ROI:[Chunk] = [0xFFFFFFFFFFFF42,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static let R2modp:[Chunk] = [0x89000000000000,0x8B,0x0,0x0,0x0]
static let MConst:Chunk = 0xBD

//  nums256 Edwards Curve
static let CURVE_Cof_I:Int = 4
static let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0]
static let CURVE_A:Int = 1
static let CURVE_B_I:Int = -15342
static let CURVE_B:[Chunk] = [0xFFFFFFFFFFC355,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFF]
static public let CURVE_Order:[Chunk] = [0x47B190EEDD4AF5,0x5AA52F59439B1A,0x4195,0x0,0x40000000]
static public let CURVE_Gx:[Chunk] = [0xDEC0902EED13DA,0x8A0EE3083586A0,0x5F69209BD60C39,0x6AEA237DCD1E3D,0x8A7514FB]
static public let CURVE_Gy:[Chunk] = [0xA616E7798A89E6,0x61D810856ED32F,0xD9A64B8010715F,0xD9D925C7CE9665,0x44D53E9F]


#endif

}

