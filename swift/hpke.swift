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

    static func LabeledExtract(_ salt:[UInt8]?,_ suite_ID:[UInt8],_ label:String,_ ikm:[UInt8]?) -> [UInt8] {
        let rfc="HPKE-v1"
        let RFC=[UInt8](rfc.utf8)
        let LABEL=[UInt8](label.utf8)
        var likm=[UInt8]();
        for i in 0..<RFC.count {
            likm.append(RFC[i])
        }
        for i in 0..<suite_ID.count {
            likm.append(suite_ID[i])
        }
        for i in 0..<LABEL.count {
            likm.append(LABEL[i])
        }
        if ikm != nil {
            for i in 0..<ikm!.count {
                likm.append(ikm![i])
            }
        }
        return HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,salt,likm)
    }

    static func LabeledExpand(_ prk:[UInt8],_ suite_ID:[UInt8],_ label:String,_ info:[UInt8]?,_ L:Int) -> [UInt8] {
        let rfc="HPKE-v1"
        let RFC=[UInt8](rfc.utf8)
        let LABEL=[UInt8](label.utf8)
        let ar = HMAC.inttoBytes(L, 2)
        var linfo=[UInt8]();
        for i in 0..<ar.count {
            linfo.append(ar[i])
        }
        for i in 0..<RFC.count {
            linfo.append(RFC[i])
        }
        for i in 0..<suite_ID.count {
            linfo.append(suite_ID[i])
        }
        for i in 0..<LABEL.count {
            linfo.append(LABEL[i])
        }
        if info != nil {
            for i in 0..<info!.count {
                linfo.append(info![i])
            }
        }
        return HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,prk,linfo)
    }

    static func ExtractAndExpand(_ config_id: Int,_ dh:[UInt8],_ context:[UInt8]) -> [UInt8] {
        let kem = config_id&255
        let txt="KEM"        
        var suite_ID=[UInt8]();
        let kem_id=HMAC.inttoBytes(kem, 2)
        let ckem=[UInt8](txt.utf8)
        for i in 0..<ckem.count {
            suite_ID.append(ckem[i])
        }
        suite_ID.append(kem_id[0])
        suite_ID.append(kem_id[1])

        let prk=LabeledExtract(nil,suite_ID,"eae_prk",dh)
        return LabeledExpand(prk,suite_ID,"shared_secret",context,CONFIG_CURVE.HASH_TYPE)
    }

    @discardableResult static public func DeriveKeyPair(_ config_id: Int,_ sk:inout [UInt8],_ pk:inout [UInt8],_ seed:[UInt8]) -> Bool {
        var counter=0
        let kem = config_id&255
        var NULLRNG : RAND? = nil

        let txt="KEM"        
        var suite_ID=[UInt8]();
        let kem_id=HMAC.inttoBytes(kem, 2)
        let ckem=[UInt8](txt.utf8)
        for i in 0..<ckem.count {
            suite_ID.append(ckem[i])
        }
        suite_ID.append(kem_id[0])
        suite_ID.append(kem_id[1])

        let prk=LabeledExtract(nil,suite_ID,"dkp_prk",seed)
        var s=[UInt8]();
        if kem==32 || kem==33 { // RFC7748
            s=LabeledExpand(prk,suite_ID,"sk",nil,ECDH.EGS);
            s.reverse();
		    if kem==32 {
			    s[ECDH.EGS-1]&=248
			    s[0]&=127
			    s[0]|=64
		    } else {
			    s[ECDH.EGS-1]&=252
			    s[0]|=128
		    }
        } else {
            var bit_mask=0xff;
            if kem==18 {
                bit_mask=1;
            }
            for _ in 0..<ECDH.EGS {
                s.append(0);
            }
            while !ECDH.IN_RANGE(s) && counter<256 {
                var info=[UInt8](repeating: 0, count:1)
                info[0]=UInt8(counter)
                s=LabeledExpand(prk,suite_ID,"candidate",info,ECDH.EGS)
                s[0] &= UInt8(bit_mask)
                counter = counter+1
            }
        }
        for i in 0..<ECDH.EGS {
            sk[i]=s[i]
        }
        ECDH.KEY_PAIR_GENERATE(&NULLRNG, &sk, &pk)
        if kem==32 || kem==33 {
            pk.reverse()
        }
        if counter<256 {
            return true
        }
        return false
    }

        var T=[UInt8](repeating: 0,count: ECDH.EFS)

    static public func ENCAP(_ config_id: Int,_ skE:[UInt8],_ pkE:[UInt8],_ pkR:[UInt8]) -> [UInt8] {
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var kemcontext=[UInt8]()

        let kem = config_id&255
  
        if kem==32 || kem==33 {
            ECDH.ECPSVDP_DH(skE, pkR.reversed(), &dh, 0)
            dh.reverse()
        } else {
            ECDH.ECPSVDP_DH(skE, pkR, &dh, 0)
        }

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)

        return ExtractAndExpand(config_id,dh,kemcontext)
    }

    static public func DECAP(_ config_id: Int,_ skR: [UInt8],_ pkE:[UInt8],_ pkR:[UInt8]) -> [UInt8] {
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var kemcontext=[UInt8]()

        let kem = config_id&255

        if kem==32 || kem==33 {
            ECDH.ECPSVDP_DH(skR, pkE.reversed(), &dh, 0)
            dh.reverse()
        } else {
            ECDH.ECPSVDP_DH(skR, pkE, &dh, 0)
        }
 
        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)

        return ExtractAndExpand(config_id,dh,kemcontext)
    }

    static public func AUTHENCAP(_ config_id: Int,_ skE:[UInt8],_ skS:[UInt8],_ pkE:[UInt8],_ pkR:[UInt8],_ pkS:[UInt8]) -> [UInt8] {
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var dh1=[UInt8](repeating: 0,count: ECDH.EFS)
        var kemcontext=[UInt8]()
        let kem = config_id&255

        if kem==32 || kem==33 {
            ECDH.ECPSVDP_DH(skE, pkR.reversed(), &dh, 0)
            ECDH.ECPSVDP_DH(skS, pkR.reversed(), &dh1, 0)
            dh.reverse()
            dh1.reverse()
        } else {
            ECDH.ECPSVDP_DH(skE, pkR, &dh, 0)
            ECDH.ECPSVDP_DH(skS, pkR, &dh1, 0)
        }

        dh.append(contentsOf: dh1)

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)
        kemcontext.append(contentsOf: pkS)
        return ExtractAndExpand(config_id,dh,kemcontext)
    }

    static public func AUTHDECAP(_ config_id: Int,_ skR:[UInt8],_ pkE: [UInt8],_ pkR:[UInt8],_ pkS: [UInt8]) -> [UInt8]  {
        var dh=[UInt8](repeating: 0,count: ECDH.EFS)
        var dh1=[UInt8](repeating: 0,count: ECDH.EFS)
        var kemcontext=[UInt8]()

        let kem = config_id&255

        if kem==32 || kem==33 {
            ECDH.ECPSVDP_DH(skR, pkE.reversed(), &dh, 0)
            ECDH.ECPSVDP_DH(skR, pkS.reversed(), &dh1, 0)
            dh.reverse() 
            dh1.reverse()
        } else {
            ECDH.ECPSVDP_DH(skR, pkE, &dh, 0)
            ECDH.ECPSVDP_DH(skR, pkS, &dh1, 0)
        }

        dh.append(contentsOf: dh1)

        kemcontext.append(contentsOf: pkE)
        kemcontext.append(contentsOf: pkR)
        kemcontext.append(contentsOf: pkS)
        return ExtractAndExpand(config_id,dh,kemcontext)
    }

    static public func KEYSCHEDULE(_ config_id: Int,_ mode: Int,_ Z: [UInt8],_ info: [UInt8],_ psk: [UInt8]?,_ pskID: [UInt8]?) -> ([UInt8],[UInt8],[UInt8]) {
        var context=[UInt8]()

        let kem = config_id&255
        let kdf=(config_id>>8)&3
        let aead=(config_id>>10)&3
        let txt="HPKE"        
        var suite_ID=[UInt8]();
        let ckem=[UInt8](txt.utf8)
        for i in 0..<ckem.count {
            suite_ID.append(ckem[i])
        }
        var num=HMAC.inttoBytes(kem, 2)
        suite_ID.append(num[0])
        suite_ID.append(num[1])
        num=HMAC.inttoBytes(kdf, 2)
        suite_ID.append(num[0])
        suite_ID.append(num[1])
        num=HMAC.inttoBytes(aead, 2)
        suite_ID.append(num[0])
        suite_ID.append(num[1])

        let ar = HMAC.inttoBytes(mode, 1)
        context.append(contentsOf: ar)

        var H=LabeledExtract(nil,suite_ID,"psk_id_hash",pskID!)
        context.append(contentsOf: H)
        H=LabeledExtract(nil,suite_ID,"info_hash",info)
        context.append(contentsOf: H)
        //H=LabeledExtract(nil,suite_ID,"psk_hash",psk!);
        
        //let secret=LabeledExtract(H,suite_ID,"secret",Z)
        let secret=LabeledExtract(Z,suite_ID,"secret",psk!);

        let key=LabeledExpand(secret,suite_ID,"key",context,CONFIG_CURVE.AESKEY)
        let nonce=LabeledExpand(secret,suite_ID,"base_nonce",context,12)
        let exp_secret=LabeledExpand(secret,suite_ID,"exp",context,CONFIG_CURVE.HASH_TYPE)

        return (key,nonce,exp_secret)
    }
}
