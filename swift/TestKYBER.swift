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

public func TestKYBER(){

        let LOOPS=100
        var RAW=[UInt8](repeating: 0,count:100)
        var R64=[UInt8](repeating: 0,count:64)
        var R32=[UInt8](repeating: 0,count:32)
        var SK=[UInt8](repeating: 0,count:KYBER.SECRET_CCA_SIZE_768)
        var PK=[UInt8](repeating: 0,count:KYBER.PUBLIC_SIZE_768)
        var SS=[UInt8](repeating: 0,count:KYBER.SHARED_SECRET_768)
        var CT=[UInt8](repeating: 0,count:KYBER.CIPHERTEXT_SIZE_768)

	    var rng=RAND()
		rng.clean()
	    for i in 0..<100 {RAW[i]=UInt8((i+1)&0xff)}
	    rng.seed(100,RAW)

        print("\nTesting KYBER Key Exchange")

        for j in 0..<LOOPS {
		    for i in 0..<64 {
			    R64[i]=rng.getByte()
		    }

            KYBER.keypair768(R64,&SK,&PK)
/*
            for i in 0..<SK.count {
                let h=String(format: "%02x",SK[i])
            	print("\(h) ", terminator: "")
        	}
            print("")
            for i in 0..<PK.count {
                let h=String(format: "%02x",PK[i])
            	print("\(h) ", terminator: "")
        	}
            print("")
*/
		    for i in 0..<32 {
			    R32[i]=rng.getByte()
		    }
            KYBER.encrypt768(R32,PK,&SS,&CT)
            print("j= \(j)")
            for i in 0..<SS.count {
                let h=String(format: "%02x",SS[i])
            	print("\(h)", terminator: "")
        	}
        	print("")
            KYBER.decrypt768(SK,CT,&SS)
            for i in 0..<SS.count {
                let h=String(format: "%02x",SS[i])
            	print("\(h)", terminator: "")
        	}
            print("") 
        } 
}

TestKYBER()

