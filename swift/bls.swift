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

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-00 */

// Minimal-signature-size variant

import Foundation
import core

public struct BLS
{
    static public let BFS=Int(CONFIG_BIG.MODBYTES)
    static public let BGS=Int(CONFIG_BIG.MODBYTES)
    static let BLS_OK:Int = 0
    static let BLS_FAIL:Int = -1

    static var G2_TAB=[FP4]()
    
    static func ceil(_ a: Int,_ b:Int) -> Int {
        return (((a)-1)/(b)+1)
    }

/* output u \in F_p */
    static func hash_to_base(_ hf: Int,_ hlen: Int,_ DST: [UInt8],_ M: [UInt8],_ ctr:Int) -> BIG {
        let q=BIG(ROM.Modulus)
        let L = ceil(q.nbits()+CONFIG_CURVE.AESKEY*8,8)

        let tag = "H2C"
        let T=[UInt8](tag.utf8)
        var INFO=[UInt8](repeating: 0,count: T.count+1)
        for i in 0..<T.count {
            INFO[i]=T[i]
        }
        INFO[T.count]=UInt8(ctr)

        let PRK=HMAC.HKDF_Extract(hf,hlen,DST,M)
        let OKM=HMAC.HKDF_Expand(hf,hlen,L,PRK,INFO)

        var dx = DBIG.fromBytes(OKM)
        let u = dx.mod(q)

        return u
    }

    /* hash a message to an ECP point, using SHA2, random oracle method */
    static public func bls_hash_to_point(_ M: [UInt8]) -> ECP
    {
        let dst = "BLS_SIG_G1-SHA256-SSWU-RO-_NUL_";
        let u=hash_to_base(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,[UInt8](dst.utf8),M,0)
        let u1=hash_to_base(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,[UInt8](dst.utf8),M,1)

        var P=ECP.hashit(u)
        let P1=ECP.hashit(u1)
        P.add(P1)
        P.cfp()
        P.affine()
        return P
    }

    static public func initit()  -> Int 
    {
        let G=ECP2.generator()
        if G.is_infinity() {return BLS_FAIL}
        G2_TAB=PAIR.precomp(G)
        return BLS_OK
    }

/* generate key pair, private key S, public key W */
    @discardableResult static public func KeyPairGenerate(_ IKM: [UInt8],_ S:inout [UInt8],_ W:inout [UInt8]) -> Int
    {
        let r = BIG(ROM.CURVE_Order)
        let L = ceil(3*ceil(r.nbits(),8),2)
        var G = ECP2.generator()
        if G.is_infinity() {return BLS_FAIL}
        let salt = "BLS-SIG-KEYGEN-SALT-"
        let info = ""

        let PRK=HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,[UInt8](salt.utf8),IKM)
        let OKM=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,PRK,[UInt8](info.utf8))
        
        var dx = DBIG.fromBytes(OKM)
        let s = dx.mod(r)

        s.toBytes(&S)      
        G=PAIR.G2mul(G,s) 
        G.toBytes(&W,true)       
        return BLS_OK
    }

/* Sign message m using private key S to produce signature SIG */

    @discardableResult static public func core_sign(_ SIG:inout [UInt8],_ M: [UInt8],_ S: [UInt8]) -> Int
    {
        var D=bls_hash_to_point(M)
        let s=BIG.fromBytes(S)
        D=PAIR.G1mul(D,s)         
        D.toBytes(&SIG,true)
        return BLS_OK
    }

/* Verify signature given message m, the signature SIG, and the public key W */

    static public func core_verify(_ SIG: [UInt8],_ M: [UInt8],_ W: [UInt8]) -> Int
    {
        let HM=bls_hash_to_point(M)    
        
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
 