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

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

*/

//
//  TestBLS.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Test Boneh-Lynn-Shacham signature API */

import Foundation
import core // comment out for Xcode
import bn254
import bls383
import bls24
import bls48

public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ");
}


public func TestBLS_bn254(_ rng: inout RAND)
{
    let BGS=bn254.BLS.BGS
    let BFS=bn254.BLS.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=2*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    var res=bn254.BLS.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }

    res=bn254.BLS.KeyPairGenerate(&rng,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }

    print("Testing bn254 curve\n")
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bn254.BLS.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bn254.BLS.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls383(_ rng: inout RAND)
{
    let BGS=bls383.BLS.BGS
    let BFS=bls383.BLS.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=2*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    var res=bls383.BLS.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls383.BLS.KeyPairGenerate(&rng,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls383 curve\n")    
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)    

    bls383.BLS.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls383.BLS.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }

}

public func TestBLS_bls24(_ rng: inout RAND)
{
    let BGS=bls24.BLS192.BGS
    let BFS=bls24.BLS192.BFS
    let G1S=BFS+1    // Group 1 Size  - compressed
    let G2S=4*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    var res=bls24.BLS192.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls24.BLS192.KeyPairGenerate(&rng,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls24 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)

    bls24.BLS192.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls24.BLS192.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }
}

public func TestBLS_bls48(_ rng: inout RAND)
{
    let BGS=bls48.BLS256.BGS
    let BFS=bls48.BLS256.BFS
    let G1S=BFS+1    // Group 1 Size - compressed
    let G2S=8*BFS+1;     // Group 2 Size - compressed
   
    var S=[UInt8](repeating: 0,count: BGS)
    var W=[UInt8](repeating: 0,count: G2S)
    var SIG=[UInt8](repeating: 0,count: G1S)

    var res=bls48.BLS256.initit()
    if res != 0 {
        print("Failed to Initialize\n")
        return
    }
    res=bls48.BLS256.KeyPairGenerate(&rng,&S,&W)
    if res != 0 {
        print("Failed to Generate Keys\n")
        return
    }
    print("Testing bls48 curve\n")      
    print("Private key : 0x",terminator: "");  printBinary(S);
    print("Public  key : 0x",terminator: "");  printBinary(W);
    let mess="This is a test message"
    print("Signing string - "+mess)
    
    bls48.BLS256.sign(&SIG,mess,S)
    print("Signature : 0x",terminator: "");  printBinary(SIG);

    res=bls48.BLS256.verify(SIG,mess,W)
    if res == 0 {
        print("Signature is OK\n");
    } else {
        print("Signature is *NOT* OK\n");
    }
}

print("Testing Boneh-Lynn-Shacham signature code\n");


var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()
    
rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)

TestBLS_bn254(&rng)
TestBLS_bls383(&rng)
TestBLS_bls24(&rng)
TestBLS_bls48(&rng)
