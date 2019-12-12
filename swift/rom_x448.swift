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
static let Modulus:[Chunk] = [0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FDFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFF]
static let R2modp:[Chunk] = [0x0,0x10,0x0,0x0,0x0,0x0,0x0,0x0,0x3000000,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let ROI:[Chunk] = [0x1FFFFFFE,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FDFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFF]
static let MConst:Chunk = 0x1

static let CURVE_A:Int = 156326
static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0xB5844F3,0x1BC61495,0x1163D548,0x1984E51B,0x3690216,0xDA4D76B,0xFA7113B,0x1FEF9944,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FF]
static public let CURVE_Gx:[Chunk] = [0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Gy:[Chunk] = [0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]

#endif

#if D64

// Base Bits= 58
static let Modulus:[Chunk] = [0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FBFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFF]
static let R2modp:[Chunk] = [0x200000000,0x0,0x0,0x0,0x3000000,0x0,0x0,0x0]
static let ROI:[Chunk] = [0x3FFFFFFFFFFFFFE,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FBFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFF]
static let MConst:Chunk = 0x1

//*** rom curve parameters *****
// Base Bits= 58

static let CURVE_A:Int = 156326
static let CURVE_Cof_I:Int = 4
static public let CURVE_Cof:[Chunk] = [0x4,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static let CURVE_B_I:Int = 0
static public let CURVE_B:[Chunk] = [0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Order:[Chunk] = [0x378C292AB5844F3,0x3309CA37163D548,0x1B49AED63690216,0x3FDF3288FA7113B,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0x3FFFFFFFFFFFFFF,0xFFFFFFFFFF]
static public let CURVE_Gx:[Chunk] = [0x5,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
static public let CURVE_Gy:[Chunk] = [0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0]
#endif

}

