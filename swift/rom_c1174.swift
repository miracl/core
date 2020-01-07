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
//  c1174 Curve Modulus
static let Modulus:[Chunk] = [0x1FFFFFF7,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static let R2modp:[Chunk] = [0x5100000,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let ROI:[Chunk] = [0x1FFFFFF6,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static let MConst:Chunk = 0x9

//*** rom curve parameters *****
// Base Bits= 29

static let CURVE_A:Int = 1
static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = -1174
static public let CURVE_B:[Chunk] = [0x1FFFFB61,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFFF]
static public let CURVE_Order:[Chunk] = [0x1166C971,0xA26A2FE,0x14C1CD22,0x12CB89BF,0x1FFFFF77,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFF]
static public let CURVE_Gx:[Chunk] = [0x1CE29EDA,0x1091F93D,0xBB35945,0x3B2D492,0x1A190C02,0x1773E014,0x111E4D0E,0x19D4611,0x37FBB]
static public let CURVE_Gy:[Chunk] = [0x1B46360E,0x6658DFC,0xFE5DAE9,0x5BDC55E,0xE0C4FE,0x208B4C2,0x1F31995A,0x105A8FF6,0x6B72F]

#endif

#if D64

// Base Bits= 56
//  c1174 Curve Modulus
static let Modulus:[Chunk] = [0xFFFFFFFFFFFFF7,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF]
static let R2modp:[Chunk] = [0x0,0x144,0x0,0x0,0x0]
static let ROI:[Chunk] = [0xFFFFFFFFFFFFF6,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF]
static let MConst:Chunk = 0x9

//*** rom curve parameters *****
// Base Bits= 56

static let CURVE_A:Int = 1
static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = -1174
static public let CURVE_B:[Chunk] = [0xFFFFFFFFFFFB61,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF]
static public let CURVE_Order:[Chunk] = [0x44D45FD166C971,0x65C4DFD3073489,0xFFFFFFFFFFF779,0xFFFFFFFFFFFFFF,0x1FFFFFF]
static public let CURVE_Gx:[Chunk] = [0x123F27BCE29EDA,0xD96A492ECD6516,0xE7C029A190C021,0xEA308C479343AE,0x37FBB0C]
static public let CURVE_Gy:[Chunk] = [0xCCB1BF9B46360E,0xDEE2AF3F976BA4,0x1169840E0C4FE2,0xD47FB7CC665684,0x6B72F82]

#endif

}

