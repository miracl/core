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

// NewHope Simple key exchange -  - see https://eprint.iacr.org/2016/1157.pdf Protocol 1

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

