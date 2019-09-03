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
//  Created by Michael Scott on 15/04/2019.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

public struct ROM{
 
#if D32
// Base Bits= 28
static let Modulus:[Chunk] = [0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF]
static let R2modp:[Chunk] = [0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0]
static let MConst:Chunk = 0x1

//*** rom curve parameters *****
// Base Bits= 28

static let CURVE_A:Int = -3
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xD940E93,0xBCBD414,0xB8F92DD,0x89F515A,0x9A7F397,0x4BCF650,0x44D5A9E,0x9D9F5E3,0x8E9FA9E,0x2]
static public let CURVE_Order:[Chunk] = [0x9D54123,0xBBF4093,0x6052B53,0xDF6B21C,0xFFF7203,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF]
static public let CURVE_Gx:[Chunk] = [0x34C74C7,0x5A45893,0x60BE171,0xBBFF26,0x9948FE3,0x466A39C,0x95F9904,0x1F19811,0x2C4AE2C,0x3]
static public let CURVE_Gy:[Chunk] = [0x139F0A0,0xDF32E52,0xA474002,0x877CC62,0x153D0A9,0xE36B692,0xC59BDCE,0xF4F6779,0xC3736A2,0xB]

#endif

#if D64

// Base Bits= 56
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFFF,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE]
static let R2modp:[Chunk] = [0x3000000040000,0xFFFFFFFE000000,0x300000002,0x10000000100,0x60000]
static let MConst:Chunk = 0x1

//*** rom curve parameters *****
// Base Bits= 56

static let CURVE_A:Int = -3
static let CURVE_Cof_I:Int = 1
static public let CURVE_Cof:[Chunk] = [0x1,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0xBCBD414D940E93,0x89F515AB8F92DD,0x4BCF6509A7F397,0x9D9F5E344D5A9E,0x28E9FA9E]
static public let CURVE_Order:[Chunk] = [0xBBF40939D54123,0xDF6B21C6052B53,0xFFFFFFFFFF7203,0xFFFFFFFFFFFFFF,0xFFFFFFFE]
static public let CURVE_Gx:[Chunk] = [0x5A4589334C74C7,0xBBFF2660BE171,0x466A39C9948FE3,0x1F1981195F9904,0x32C4AE2C]
static public let CURVE_Gy:[Chunk] = [0xDF32E52139F0A0,0x877CC62A474002,0xE36B692153D0A9,0xF4F6779C59BDCE,0xBC3736A2]

#endif

}

