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

    static func LabeledExtract(_ salt:[UInt8]?,_ label:String,_ ikm:[UInt8]?) -> [UInt8] {
        let rfc="RFCXX"+"XX "
        let prefix=[UInt8]((rfc+label).utf8)
        var likm=[UInt8]();
        for i in 0..<prefix.count {
            likm.append(prefix[i])
        }
        if ikm != nil {
            for i in 0..<ikm!.count {
                likm.append(ikm![i])
            }
        }
        return HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,salt,likm)
    }

    static func LabeledExpand(_ prk:[UInt8],_ label:String,_ info:[UInt8],_ L:Int) -> [UInt8] {
        let rfc="RFCXX"+"XX "
        let prefix=[UInt8]((rfc+label).utf8)
        let ar = HMAC.inttoBytes(L, 2)
        var linfo=[UInt8]();
        for i in 0..<ar.count {
            linfo.append(ar[i])
        }
        for i in 0..<prefix.count {
            linfo.append(prefix[i])
        }
        for i in 0..<info.count {
            linfo.append(info[i])
        }
        return HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,prk,linfo)
    }

    static func ExtractAndExpand(_ dh:[UInt8],_ context:[UInt8]) -> [UInt8] {
        let prk=LabeledExtract(nil,"dh",dh)
        return LabeledExpand(prk,"prk",context,CONFIG_CURVE.HASH_TYPE)
    }

    static public func ENCAP(_ config_id: Int,_ ske:[UInt8],_ pkE:inout [UInt8],_ pkR:inout [UInt8]) -> [UInt8] {
        var skE=[UInt8](repeating: 0,count: ECDH.EGS)
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var kemcontext=[UInt8]();

        var NULLRNG : RAND? = nil
        let kem = config_id&255
  
        for i in 0..<ECDH.EGS {
            skE[i]=ske[i]
        }
        if kem==32 || kem==33 {
            reverse(&skE)
            if kem==32 {
                skE[ECDH.EGS-1]&=248
                skE[0]&=127
                skE[0]|=64
            } else {
                skE[ECDH.EGS-1]&=252
                skE[0]|=128
            }
        }
        
        ECDH.KEY_PAIR_GENERATE(&NULLRNG, &skE, &pkE)
        if kem==32 || kem==33 {
            reverse(&pkR);
        }
        ECDH.ECPSVDP_DH(skE, pkR, &dh)
        if kem==32 || kem==33 {
            reverse(&pkR)
            reverse(&pkE)
            reverse(&dh)
        }

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)

        return ExtractAndExpand(dh,kemcontext)
    }

    static public func DECAP(_ config_id: Int,_ pkE:inout [UInt8],_ skr: [UInt8]) -> [UInt8] {
        var skR=[UInt8](repeating: 0,count: ECDH.EGS)
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var pkR=[UInt8](); 
        pkR.append(contentsOf: pkE)
        var kemcontext=[UInt8]();

        var NULLRNG : RAND? = nil

        let kem = config_id&255
        for i in 0..<ECDH.EGS {
            skR[i]=skr[i]
        }
        if kem==32 || kem==33 {
            reverse(&skR)
            reverse(&pkE)
            if kem==32 {
                skR[ECDH.EGS-1]&=248
                skR[0]&=127
                skR[0]|=64
            } else {
                skR[ECDH.EGS-1]&=252
                skR[0]|=128
            }
        }
        ECDH.ECPSVDP_DH(skR, pkE, &dh)
        if kem==32 || kem==33 {
            reverse(&pkE)
            reverse(&dh)
        }
        ECDH.KEY_PAIR_GENERATE(&NULLRNG,&skR,&pkR)
        if kem==32 || kem==33 {
            reverse(&pkR)
        }

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)

        return ExtractAndExpand(dh,kemcontext)
    }

    static public func AUTHENCAP(_ config_id: Int,_ ske:[UInt8],_ pkE:inout [UInt8],_ pkR:inout [UInt8],_ ski:inout [UInt8]) -> [UInt8]{
        var skE=[UInt8](repeating: 0,count: ECDH.EGS)
        var skS=[UInt8](repeating: 0,count: ECDH.EGS)
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var dh1=[UInt8](repeating: 0,count: ECDH.EFS)
        var pkS=[UInt8]()
        pkS.append(contentsOf: pkE)
        var kemcontext=[UInt8]();

        var NULLRNG : RAND? = nil
        let kem = config_id&255

        for i in 0..<ECDH.EGS {
            skE[i]=ske[i]
            skS[i]=ski[i]
        }
        if kem==32 || kem==33 {
            reverse(&skE)
            reverse(&skS)
            if kem==32 {
                skE[ECDH.EGS-1]&=248
                skE[0]&=127
                skE[0]|=64
                skS[ECDH.EGS-1]&=248
                skS[0]&=127
                skS[0]|=64
            } else {
                skE[ECDH.EGS-1]&=252
                skE[0]|=128
                skS[ECDH.EGS-1]&=252
                skS[0]|=128
            }
        }
        
        ECDH.KEY_PAIR_GENERATE(&NULLRNG, &skE, &pkE)
        if kem==32 || kem==33 {
            reverse(&pkR)
        }
        ECDH.ECPSVDP_DH(skE, pkR, &dh)
        ECDH.ECPSVDP_DH(skS, pkR, &dh1)
        if kem==32 || kem==33 {
            reverse(&dh)
            reverse(&dh1)
        }
        dh.append(contentsOf: dh1)

       ECDH.KEY_PAIR_GENERATE(&NULLRNG, &skS, &pkS)    

        if kem==32 || kem==33 {
            reverse(&pkR)
            reverse(&pkE)
            reverse(&pkS)
        }

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)
        kemcontext.append(contentsOf: pkS)
        return ExtractAndExpand(dh,kemcontext)
    }

    static public func AUTHDECAP(_ config_id: Int,_ pkE: inout [UInt8],_ skr:[UInt8],_ pkS: inout [UInt8]) -> [UInt8]  {
        var skR=[UInt8](repeating: 0,count: ECDH.EGS)
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var dh1=[UInt8](repeating: 0,count: ECDH.EFS)
        var pkR=[UInt8]()
        pkR.append(contentsOf: pkE)
        var kemcontext=[UInt8]();

        var NULLRNG : RAND? = nil
        let kem = config_id&255
        for i in 0..<ECDH.EGS {
            skR[i]=skr[i]
        }
        if kem==32 || kem==33 {
            reverse(&skR)
            reverse(&pkE)
            reverse(&pkS)
            if kem==32 {
                skR[ECDH.EGS-1]&=248
                skR[0]&=127
                skR[0]|=64
            } else {
                skR[ECDH.EGS-1]&=252
                skR[0]|=128
            }
        }
        ECDH.ECPSVDP_DH(skR, pkE, &dh)
        ECDH.ECPSVDP_DH(skR, pkS, &dh1)

        if kem==32 || kem==33 {
            reverse(&dh)
            reverse(&dh1)
        }

        dh.append(contentsOf: dh1)

        ECDH.KEY_PAIR_GENERATE(&NULLRNG, &skR, &pkR)    

        if kem==32 || kem==33 {
            reverse(&pkR)
            reverse(&pkE)
            reverse(&pkS)
        }
        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)
        kemcontext.append(contentsOf: pkS)
        return ExtractAndExpand(dh,kemcontext)
    }

    static public func KEYSCHEDULE(_ config_id: Int,_ mode: Int,_ Z: [UInt8],_ info: [UInt8],_ psk: [UInt8]?,_ pskID: [UInt8]?) -> ([UInt8],[UInt8],[UInt8]) {
        var context=[UInt8]()

        let kem_id=config_id&255
        let kdf_id=(config_id>>8)&3
        let aead_id=(config_id>>10)&3

        var ar=HMAC.inttoBytes(kem_id, 2)
        context.append(contentsOf: ar)

        ar=HMAC.inttoBytes(kdf_id, 2)
        context.append(contentsOf: ar)

        ar=HMAC.inttoBytes(aead_id, 2)
        context.append(contentsOf: ar)

        ar = HMAC.inttoBytes(mode, 1)
        context.append(contentsOf: ar)

        let zeros=[UInt8](repeating: 0,count: CONFIG_CURVE.HASH_TYPE)

        var H=LabeledExtract(zeros,"pskID_hash",pskID!)
        context.append(contentsOf: H)
        H=LabeledExtract(zeros,"info",info)
        context.append(contentsOf: H)

        if psk==nil {
            H=LabeledExtract(zeros,"psk_hash",zeros);
        } else {
            H=LabeledExtract(zeros,"psk_hash",psk!);
        }
        let secret=LabeledExtract(H,"zz",Z)

        let key=LabeledExpand(secret,"key",context,CONFIG_CURVE.AESKEY)
        let nonce=LabeledExpand(secret,"nonce",context,12)
        let exp_secret=LabeledExpand(secret,"exp",context,CONFIG_CURVE.HASH_TYPE)

        return (key,nonce,exp_secret)
    }
}
