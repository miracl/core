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
//  bls.swift
//
//  Created by Michael Scott on 03/12/2018.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* Boneh-Lynn-Shacham Signature API Functions */

import Foundation
import core

public struct BLS
{
    static public let BFS=Int(CONFIG_BIG.MODBYTES)
    static public let BGS=Int(CONFIG_BIG.MODBYTES)
    static let BLS_OK:Int = 0
    static let BLS_FAIL:Int = -1

    static var G2_TAB=[FP4]()
    
    static public func initit()  -> Int 
    {
        let G=ECP2.generator()
        if G.is_infinity() {return BLS_FAIL}
        G2_TAB=PAIR.precomp(G)
        return BLS_OK
    }
/* hash a message to an ECP point, using SHA3 */

    static public func bls_hashit(_ m: String) -> ECP
    {
        var sh=SHA3(SHA3.SHAKE256)
        var hm=[UInt8](repeating: 0,count: BFS)  
        let t=[UInt8](m.utf8)     
        for i in 0..<t.count {sh.process(t[i])}
        sh.shake(&hm,BFS)
        let P=ECP.mapit(hm) 
        return P
    }

/* generate key pair, private key S, public key W */

    @discardableResult static public func KeyPairGenerate(_ rng: inout RAND,_ S:inout [UInt8],_ W:inout [UInt8]) -> Int
    {
        var G=ECP2.generator()
        if G.is_infinity() {return BLS_FAIL}
        let q=BIG(ROM.CURVE_Order)
        let s=BIG.randtrunc(q,16*CONFIG_CURVE.AESKEY,&rng)

        s.toBytes(&S)      
        G=PAIR.G2mul(G,s) 
        G.toBytes(&W,true)       
        return BLS_OK
    }

/* Sign message m using private key S to produce signature SIG */

    @discardableResult static public func sign(_ SIG:inout [UInt8],_ m: String,_ S: [UInt8]) -> Int
    {
        var D=bls_hashit(m)
        let s=BIG.fromBytes(S)
        D=PAIR.G1mul(D,s)         
        D.toBytes(&SIG,true)
        return BLS_OK
    }

/* Verify signature given message m, the signature SIG, and the public key W */

    static public func verify(_ SIG: [UInt8],_ m: String,_ W: [UInt8]) -> Int
    {
        let HM=bls_hashit(m)    
        
        var D=ECP.fromBytes(SIG)  
        if !PAIR.G1member(D) {return BLS_FAIL}
        D.neg()

        let PK=ECP2.fromBytes(W)

// Use new multi-pairing mechanism 
        var r=PAIR.initmp()
        //PAIR.another(&r,G,D)
        PAIR.another_pc(&r,G2_TAB,D)        
        PAIR.another(&r,PK,HM)
        var v=PAIR.miller(&r)

//.. or alternatively
//        var v=PAIR.ate2(G,D,PK,HM)

        v=PAIR.fexp(v)
    
        if v.isunity() {
            return BLS_OK
        } else {
            return BLS_FAIL
        }
    }
}
 