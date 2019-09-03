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
//  TestNHS.swift - Tests NewHope Simple API
//  swift -I. -L. -lcore TestNHS.swift
// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)


import Foundation
import core // comment out for Xcode

public func TestNHS()
{
	var S=[UInt8](repeating:0,count:1792)
	var SB=[UInt8](repeating:0,count:1824)
	var UC=[UInt8](repeating:0,count:2176)
	var KEYA=[UInt8](repeating:0,count:32)
	var KEYB=[UInt8](repeating:0,count:32)

        print("\nTesting New Hope Key Exchange")

	var RAW=[UInt8](repeating: 0,count: 100)
	var srng=RAND()
				var crng=RAND()
				crng.clean()

	srng.clean()
	for i in 0..<100 {RAW[i]=UInt8((i+1)&0xff)}
	srng.seed(100,RAW)

				for i in 0..<100 {RAW[i]=UInt8((i+2)&0xff)}
				crng.seed(100,RAW)


	NHS.SERVER_1(&srng,&SB,&S)
				NHS.CLIENT(&crng,SB,&UC,&KEYB)

 			   	for i in 0..<32 {
            				let h=String(KEYB[i],radix:16)
            				print("\(h)", terminator: "")
        			}
        			print("")

	NHS.SERVER_2(S,UC,&KEYA)

	for i in 0..<32 {
            	let h=String(KEYA[i],radix:16)
            	print("\(h)", terminator: "")
        }
        print("")
}

TestNHS()

