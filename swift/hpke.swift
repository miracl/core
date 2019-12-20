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
//  hpke.swift
//
//  Created by Michael Scott on 19/12/2019.
//  Copyright (c) 2019 Michael Scott. All rights reserved.
//

import core

/* HPKE high-level API functions  */
/*
public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ")
}
*/
public struct HPKE
{
    /* reverse array */
    static func reverse(_ x:inout [UInt8])
    {
            let lx=x.count;
            for i in 0..<(lx/2) {
                let ch=x[i]
                x[i]=x[lx-i-1]
                x[lx-i-1]=ch
            }
    }

    static public func ENCAP(_ config_id: Int,_ RNG: inout RAND?,_ ske:[UInt8],_ pkE:inout [UInt8],_ pkR:inout [UInt8]) -> [UInt8] {
        var skE=[UInt8](repeating: 0,count: ECDH.EGS)
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        let kem = config_id&7
        if RNG == nil {
            for i in 0..<ECDH.EGS {
                skE[i]=ske[i]
            }
            if kem==2 || kem==4 {
                reverse(&skE)
                if kem==2 {
                    skE[ECDH.EGS-1]&=248
                    skE[0]&=127
                    skE[0]|=64
                } else {
                    skE[ECDH.EGS-1]&=252
                    skE[0]|=128
                }
            }
        }
        ECDH.KEY_PAIR_GENERATE(&RNG, &skE, &pkE)
        if kem==2 || kem==4 {
            reverse(&pkR);
        }
        ECDH.ECPSVDP_DH(skE, pkR, &Z)
        if kem==2 || kem==4 {
            reverse(&pkR)
            reverse(&pkE)
            reverse(&Z)
        }
        return Z
    }

    static public func DECAP(_ config_id: Int,_ pkE:inout [UInt8],_ skr: [UInt8]) -> [UInt8] {
        var skR=[UInt8](repeating: 0,count: ECDH.EGS)
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        let kem = config_id&7
        for i in 0..<ECDH.EGS {
            skR[i]=skr[i]
        }
        if kem==2 || kem==4 {
            reverse(&skR)
            reverse(&pkE)
            if kem==2 {
                skR[ECDH.EGS-1]&=248
                skR[0]&=127
                skR[0]|=64
            } else {
                skR[ECDH.EGS-1]&=252
                skR[0]|=128
            }
        }
        ECDH.ECPSVDP_DH(skR, pkE, &Z)
        if kem==2 || kem==4 {
            reverse(&pkE)
            reverse(&Z)
        }
        return Z
    }

    static public func AUTHENCAP(_ config_id: Int,_ RNG: inout RAND?,_ ske:[UInt8],_ pkE:inout [UInt8],_ pkR:inout [UInt8],_ ski:inout [UInt8]) -> [UInt8]{
        var skE=[UInt8](repeating: 0,count: ECDH.EGS)
        var skI=[UInt8](repeating: 0,count: ECDH.EGS)
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        var NZ=[UInt8](repeating: 0,count: ECDH.EFS)
        let kem = config_id&7

        if RNG == nil {
            for i in 0..<ECDH.EGS {
                skE[i]=ske[i]
                skI[i]=ski[i]
            }
            if kem==2 || kem==4 {
                reverse(&skE)
                reverse(&skI)
                if kem==2 {
                    skE[ECDH.EGS-1]&=248
                    skE[0]&=127
                    skE[0]|=64
                    skI[ECDH.EGS-1]&=248
                    skI[0]&=127
                    skI[0]|=64
                } else {
                    skE[ECDH.EGS-1]&=252
                    skE[0]|=128
                    skI[ECDH.EGS-1]&=252
                    skI[0]|=128
                }
            }
        }
        ECDH.KEY_PAIR_GENERATE(&RNG, &skE, &pkE)
        if kem==2 || kem==4 {
            reverse(&pkR)
        }
        ECDH.ECPSVDP_DH(skE, pkR, &Z)
        ECDH.ECPSVDP_DH(skI, pkR, &NZ)
        if kem==2 || kem==4 {
            reverse(&pkR)
            reverse(&pkE)
            reverse(&Z)
            reverse(&NZ)
        }

        for i in 0..<ECDH.EFS {
            Z.append(NZ[i])
        }
        return Z
    }

    static public func AUTHDECAP(_ config_id: Int,_ pkE: inout [UInt8],_ skr:[UInt8],_ pkI: inout [UInt8]) -> [UInt8]  {
        var skR=[UInt8](repeating: 0,count: ECDH.EGS)
        var Z=[UInt8](repeating: 0,count: ECDH.EFS)
        var NZ=[UInt8](repeating: 0,count: ECDH.EFS)
        let kem = config_id&7
        for i in 0..<ECDH.EGS {
            skR[i]=skr[i]
        }
        if kem==2 || kem==4 {
            reverse(&skR)
            reverse(&pkE)
            reverse(&pkI)
            if kem==2 {
                skR[ECDH.EGS-1]&=248
                skR[0]&=127
                skR[0]|=64
            } else {
                skR[ECDH.EGS-1]&=252
                skR[0]|=128
            }
        }
        ECDH.ECPSVDP_DH(skR, pkE, &Z)
        ECDH.ECPSVDP_DH(skR, pkI, &NZ)
        if kem==2 || kem==4 {
            reverse(&pkI)
            reverse(&pkE)
            reverse(&Z)
            reverse(&NZ)
        }
        for i in 0..<ECDH.EFS {
            Z.append(NZ[i])
        }
        return Z
    }

    static public func KEYSCHEDULE(_ config_id: Int,_ mode: Int,_ pkR: [UInt8],_ Z: [UInt8],_ pkE: [UInt8],_ info: [UInt8],_ psk: [UInt8]?,_ pskID: [UInt8]?,_ pkI: [UInt8]?) -> ([UInt8],[UInt8]) {
        let ctxlen=7+3*pkR.count+2*CONFIG_CURVE.HASH_TYPE
        var context=[UInt8](repeating: 0,count: ctxlen)

        let kem_id=config_id&7
        let kdf_id=(config_id>>3)&3
        let aead_id=(config_id>>5)&3
        var k=0

        var ar = HMAC.inttoBytes(mode, 1)
        for i in 0..<ar.count {
            context[k] = ar[i]; k+=1
        }
        ar=HMAC.inttoBytes(kem_id, 2)
        for i in 0..<ar.count {
            context[k] = ar[i]; k+=1
        }
        ar=HMAC.inttoBytes(kdf_id, 2)
        for i in 0..<ar.count {
            context[k] = ar[i]; k+=1
        }
        ar=HMAC.inttoBytes(aead_id, 2)
        for i in 0..<ar.count {
            context[k] = ar[i]; k+=1
        }

        for i in 0..<pkE.count {
            context[k] = pkE[i]; k+=1
        }
        for i in 0..<pkR.count {
            context[k] = pkR[i]; k+=1
        }
        if pkI==nil {
            for _ in 0..<pkR.count {
                context[k] = 0; k+=1
            }
        } else {
            for i in 0..<pkI!.count {
                context[k] = pkI![i]; k+=1
            }
        }
        var H=HMAC.SPhashit(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,pskID!)
        for i in 0..<CONFIG_CURVE.HASH_TYPE {
            context[k] = H[i]; k+=1
        }
        H=HMAC.SPhashit(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,info)
        for i in 0..<CONFIG_CURVE.HASH_TYPE {
            context[k] = H[i]; k+=1
        }

        //print("Context = ",terminator:""); printBinary(context)
        let secret=HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,psk!,Z)
        //print("Secret = ",terminator:""); printBinary(secret)

        var full_context=[UInt8]();

        var pp=String("hpke key")
        ar=[UInt8]( (pp).utf8 )

        for i in 0..<ar.count {
            full_context.append(ar[i])
        }
        for i in 0..<ctxlen {
            full_context.append(context[i])
        }
        let key=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,CONFIG_CURVE.AESKEY,secret,full_context)
        full_context=[UInt8]();
        pp="hpke nonce"
        ar=[UInt8]( (pp).utf8 )
        for i in 0..<ar.count {
            full_context.append(ar[i])
        }
        for i in 0..<ctxlen {
            full_context.append(context[i])
        }
        let nonce=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,12,secret,full_context)

        return (key,nonce)
    }
}
