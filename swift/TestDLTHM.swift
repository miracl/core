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

/* test driver and function exerciser for KYBER API Functions */

import Foundation
import core // comment out for Xcode

public func TestDLTHM(){
        let LOOPS=100
        var RAW=[UInt8](repeating: 0,count:100)
        var TAU=[UInt8](repeating: 0,count:32)
        var SK=[UInt8](repeating:0,count:DILITHIUM.SK_SIZE_3)
        var PK=[UInt8](repeating:0,count:DILITHIUM.PK_SIZE_3)
        var SIG=[UInt8](repeating:0,count:DILITHIUM.SIG_SIZE_3)

	    var rng=RAND()
		rng.clean()
	    for i in 0..<100 {RAW[i]=UInt8((i+1)&0xff)}
	    rng.seed(100,RAW)

        print("\nTesting Dilithium Signature")
        var tats=0
        for _ in 0..<LOOPS {
            let mess="Hello World"
            for i in 0..<32 {
                TAU[i]=rng.getByte()
            }
            DILITHIUM.keypair_3(TAU,&SK,&PK)

/*
            for i in 0..<SK.count {
                let h=String(format: "%02x",SK[i])
            	print("\(h)", terminator: "")
        	}
            print("")
            for i in 0..<PK.count {
                let h=String(format: "%02x",PK[i])
            	print("\(h)", terminator: "")
        	}
            print("")
*/

            let attempts=DILITHIUM.signature_3(SK,[UInt8](mess.utf8),&SIG)
            tats += attempts
            let h=String(format: "Signature %d bits created on attempt %d\n",8*SIG.count,attempts)
            print("\(h)", terminator: "") 

            let result=DILITHIUM.verify_3(PK,[UInt8](mess.utf8),SIG)
            if result {
                print("Signature is verified")
            } else {
                print("Signature is NOT verified")
                break
            }

        }
        if LOOPS > 1 {
            let h=String(format: "Average = %d\n",tats/LOOPS)
            print("\(h)", terminator: "") 
        }
}

TestDLTHM()
