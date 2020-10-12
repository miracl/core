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
//  hmac.swift
//
//  Created by Michael Scott on 30/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* HMAC functions  */

public struct HMAC
{
    static public let MC_SHA2=2
    static public let MC_SHA3=3
    static public let SHA256=32
    static public let SHA384=48
    static public let SHA512=64

    static func ceil(_ a: Int,_ b:Int) -> Int {
        return (((a)-1)/(b)+1)
    }

    /* Convert Integer to n-byte array */
    public static func inttoBytes(_ n: Int,_ len:Int) -> [UInt8]
    {
        var b=[UInt8](repeating: 0,count: len)
        var nn=n

        var i=len;
        while (nn>0 && i>0)
        {
            i -= 1;
            b[i]=UInt8(nn&0xff);
            nn /= 256;
        }
        return b;
    }

    public static func GPhashit(_ hf: Int, _ sha: Int,_ pad:Int,_ zpad:Int,_ A:[UInt8]?,_ n: Int32,_ B:[UInt8]?) -> [UInt8]
    {
        var R=[UInt8]()
        if hf == MC_SHA2 {
            if sha==32
            {
                var H=HASH256()
                for _ in 0 ..< zpad {
                    H.process(0)
                }
                if A != nil {H.process_array(A!)}
                if n>=0 {H.process_num(n)}
                if B != nil {H.process_array(B!)}
                R=H.hash()
            }
            if sha==48
            {
                var H=HASH384()
                for _ in 0 ..< zpad {
                    H.process(0)
                }
                if A != nil {H.process_array(A!)}
                if n>=0 {H.process_num(n)}
                if B != nil {H.process_array(B!)}
                R=H.hash()
            }
            if sha==64
            {
                var H=HASH512()
                for _ in 0 ..< zpad {
                    H.process(0)
                }
                if A != nil {H.process_array(A!)}
                if n>=0 {H.process_num(n)}
                if B != nil {H.process_array(B!)}
                R=H.hash()
            }
        }
        if hf == MC_SHA3 {
            var H=SHA3(sha)
            for _ in 0 ..< zpad {
                H.process(0)
            }
            if A != nil {H.process_array(A!)}
            if n>=0 {H.process_num(n)}
            if B != nil {H.process_array(B!)}
            R=H.hash()
        }


        if R.isEmpty || pad==0 {return R}
        var W=[UInt8](repeating: 0,count: pad)
        if pad<=sha
        {
            for i in 0 ..< pad {W[i]=R[i]}
        }
        else
        {
            for i in 0 ..< sha {W[i+pad-sha]=R[i]}
        }
        return W
    }

    public static func SPhashit(_ hf: Int, _ sha: Int,_ A:[UInt8]?) -> [UInt8]
    {
        return GPhashit(hf,sha,0,0,A!,-1,nil)
    }
    /* Key Derivation Functions */
    /* Input octet Z */
    /* Output key of length olen */

    static public func KDF2(_ hf: Int, _ sha:Int,_ Z:[UInt8],_ P:[UInt8]?,_ olen:Int) -> [UInt8]
    {
    /* NOTE: the parameter olen is the length of the output k in bytes */
        let hlen=sha
        var K=[UInt8](repeating: 0,count: olen)
        var k=0;

        var cthreshold=olen/hlen; if (olen%hlen) != 0 {cthreshold += 1}

        for counter in 1...cthreshold
        {
            let B=HMAC.GPhashit(hf, sha, 0, 0, Z,Int32(counter),P)
            if k+hlen>olen {for i in 0 ..< olen%hlen {K[k]=B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k]=B[i]; k+=1}}
        }
        return K;
    }

    /* Password based Key Derivation Function */
    /* Input password p, salt s, and repeat count */
    /* Output key of length olen */
    static public func PBKDF2(_ hf: Int, _ sha:Int,_ Pass:[UInt8],_ Salt:[UInt8],_ rep:Int,_ olen:Int) -> [UInt8]
    {
        var d=olen/sha;
        if (olen%sha) != 0 {d+=1}
        var F=[UInt8](repeating: 0,count: sha)
        var U=[UInt8](repeating: 0,count: sha)
        var S=[UInt8](repeating: 0,count: Salt.count+4)

        var K=[UInt8](repeating: 0,count: d*sha)

        var opt=0;

        for i in 1...d
        {
            for j in 0 ..< Salt.count {S[j]=Salt[j]}
            let N=HMAC.inttoBytes(i,4);
            for j in 0 ..< 4 {S[Salt.count+j]=N[j]}

            HMAC1(hf, sha,&F,sha,Pass,S);

            for j in 0 ..< sha {U[j]=F[j]}
            for _ in 2...rep
            {
                HMAC1(hf, sha,&U,sha,Pass,U);
                for k in 0 ..< sha {F[k]^=U[k]}
            }
            for j in 0 ..< sha {K[opt]=F[j]; opt+=1}
        }
        var key=[UInt8](repeating: 0,count: olen)
        for i in 0 ..< olen {key[i]=K[i]}
        return key;
    }

    static func blksize(_ hf: Int, _ sha: Int) -> Int
    {
        var b=0
        if hf == MC_SHA2 {
            b=64
            if sha>32 {b=128}
        }
        if hf == MC_SHA3 {
            b=200-2*sha
        }
        return b
    }

    /* Calculate HMAC of m using key k. HMAC is tag of length olen */
    @discardableResult static public func HMAC1(_ hf: Int, _ sha: Int,_ tag:inout [UInt8],_ olen: Int, _ K:[UInt8],_ M:[UInt8]) -> Int
    {
    /* Input is from an octet m        *
    * olen is requested output length in bytes. k is the key  *
    * The output is the calculated tag */

        let b=HMAC.blksize(hf,sha)
        if b == 0 {
            return 0
        }

        var K0=[UInt8](repeating: 0,count: b)

        var B=[UInt8]();

        if (K.count > b)
        {
            B=HMAC.GPhashit(hf,sha,0 ,0,K,-1,nil)
            for i in 0 ..< sha {K0[i]=B[i]}
        }
        else
        {
            for i in 0 ..< K.count {K0[i]=K[i]}
        }
        for i in 0 ..< b {K0[i]^=0x36}

        B=HMAC.GPhashit(hf, sha, 0, 0,K0,-1,M)

        for i in 0 ..< b {K0[i]^=0x6a}
        B=HMAC.GPhashit(hf, sha, olen,0,K0,-1,B)

        for i in 0 ..< olen {tag[i]=B[i]}

        return 1;
    }

    static public func HKDF_Extract(_ hf: Int, _ hlen:Int, _ SALT: [UInt8]?, _ IKM: [UInt8])  -> [UInt8]
    {
        var PRK=[UInt8](repeating:0,count:hlen)
        if SALT == nil {
            let H=[UInt8](repeating: 0,count: hlen)
            HMAC1(hf,hlen,&PRK,hlen,H,IKM)
        } else {
            HMAC1(hf,hlen,&PRK,hlen,SALT!,IKM)
        }
        return PRK
    }

    static public func HKDF_Expand(_ hf: Int, _ hlen:Int, _ olen:Int, _ PRK: [UInt8], _ INFO:[UInt8]) -> [UInt8]
    {
        let n=olen/hlen
        let flen=olen%hlen

        var t=[UInt8](repeating: 0,count: INFO.count+1)
        var k=[UInt8](repeating: 0,count: 64)
        var OKM=[UInt8](repeating: 0,count: olen)

        var l=0;
        var m=0;
        if n>0 {
            for i in 1...n {
                for j in 0..<INFO.count {
                    t[l]=INFO[j]; l=l+1
                }
                t[l]=UInt8(i); l=l+1
                HMAC1(hf,hlen,&k,hlen,PRK,t)
                l=0
                if i==1 {t=[UInt8](repeating: 0,count: hlen+INFO.count+1)}
                for j in 0..<hlen {
                    OKM[m]=k[j]; m=m+1
                    t[l]=k[j]; l=l+1
                }
            }
        }
        if flen>0 {
            for j in 0..<INFO.count {
                t[l]=INFO[j]; l=l+1
            }
            t[l]=UInt8(n+1); l=l+1;
            HMAC1(hf,hlen,&k,flen,PRK,t);
            for j in 0..<flen {
                OKM[m]=k[j]; m=m+1;
            }
        }
        return OKM
    }

    static public func XOF_Expand(_ hlen:Int, _ olen:Int, _ DST: [UInt8], _ MSG:[UInt8]) -> [UInt8]
    {
        var OKM=[UInt8](repeating: 0,count: olen)
        var H=SHA3(hlen)
	    for i in 0..<MSG.count {
		    H.process(MSG[i])
	    }
        H.process(UInt8((olen>>8)&0xff))
        H.process(UInt8(olen&0xff))

        for i in 0..<DST.count {
            H.process(DST[i])
        }
        H.process(UInt8(DST.count & 0xff))

        H.shake(&OKM,olen)
        return OKM
    }

    static public func XMD_Expand(_ hf: Int,_ hlen:Int, _ olen:Int, _ DST: [UInt8], _ MSG:[UInt8]) -> [UInt8]
    {
        var OKM=[UInt8](repeating: 0,count: olen)
        var TMP=[UInt8](repeating: 0,count: DST.count+4)
	    let ell=ceil(olen,hlen)
	    let blk=blksize(hf,hlen)
	    TMP[0]=UInt8((olen >> 8) & 0xff)
	    TMP[1]=UInt8(olen & 0xff)
	    TMP[2]=UInt8(0)

	    for j in 0..<DST.count {
		    TMP[3+j]=DST[j]
	    }
	    TMP[3+DST.count]=UInt8(DST.count & 0xff)

	    let H0=HMAC.GPhashit(hf, hlen, 0, blk, MSG, -1, TMP)
        var H1=[UInt8](repeating: 0,count: hlen)
        var TMP2=[UInt8](repeating: 0,count: DST.count+2)

        var k=0
        for i in 1...ell {
            for j in 0..<hlen {
                H1[j]^=H0[j]
            }          
            TMP2[0]=UInt8(i)

            for j in 0..<DST.count {
                TMP2[1+j]=DST[j]
            }
            TMP2[1+DST.count]=UInt8(DST.count & 0xff)

            H1=HMAC.GPhashit(hf, hlen, 0, 0, H1, -1, TMP2)
            for j in 0..<hlen  {
                OKM[k]=H1[j]
                k+=1
                if k==olen {
                    break
                }
            }
        }     
        return OKM
    }

    /* Mask Generation Function */

    static func MGF1(_ sha: Int,_ Z: [UInt8],_ olen:Int,_ K:inout [UInt8])
    {
        let hlen=sha

        var k=0;
        for i in 0 ..< K.count {K[i]=0}

        var cthreshold=Int32(olen/hlen); if (olen%hlen != 0) {cthreshold += 1}
        for counter:Int32 in 0 ..< cthreshold
        {
            let B=HMAC.GPhashit(HMAC.MC_SHA2,sha,0,0,Z,counter,nil)

            if (k+hlen>olen) {for i in 0 ..< olen%hlen {K[k]=B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k]=B[i]; k+=1}}
        }
    }

    static func MGF1XOR(_ sha: Int,_ Z: [UInt8],_ olen:Int,_ K:inout [UInt8])
    {
        let hlen=sha

        var k=0;

        var cthreshold=Int32(olen/hlen); if (olen%hlen != 0) {cthreshold += 1}
        for counter:Int32 in 0 ..< cthreshold
        {
            let B=HMAC.GPhashit(HMAC.MC_SHA2,sha,0,0,Z,counter,nil)

            if (k+hlen>olen) {for i in 0 ..< olen%hlen {K[k] ^= B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k] ^= B[i]; k+=1}}
        }
    }

    static let SHA256ID:[UInt8]=[0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20]
    static let SHA384ID:[UInt8]=[0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30]
    static let SHA512ID:[UInt8]=[0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40]

    /* PKCS 1.5 padding of a message to be signed */
    @discardableResult static public func PKCS15(_ sha:Int,_ m:[UInt8],_ w:inout [UInt8],_ RFS:Int)->Bool
    {
        let hlen=sha
        let olen=RFS
        let idlen=19;

        if olen<idlen+hlen+10 {return false}
        let H=HMAC.SPhashit(HMAC.MC_SHA2,sha,m)

        for i in 0 ..< w.count {w[i]=0}

        w[0]=0
        w[1]=1
        var i=2
        var j=0

        while j<olen-idlen-hlen-3
        {
            w[i]=0xff
            i+=1; j+=1
        }
        w[i]=0; i+=1;

        if hlen==HMAC.SHA256
        {
            for j in 0 ..< idlen {w[i]=SHA256ID[j]; i+=1}
        }
        if hlen==HMAC.SHA384
        {
            for j in 0 ..< idlen {w[i]=SHA384ID[j]; i+=1}
        }
        if hlen==HMAC.SHA512
        {
            for j in 0 ..< idlen {w[i]=SHA512ID[j]; i+=1}
        }

        for j in 0 ..< hlen {w[i]=H[j];i+=1}

        return true
    }


    static public func PSS_ENCODE(_ sha:Int,_ m:[UInt8],_ rng: inout RAND,_ RFS:Int) -> [UInt8]
    {
        let emlen=RFS
        let embits=8*emlen-1
        let hlen=sha
        var SALT=[UInt8](repeating: 0,count: hlen)
        for i in 0 ..< hlen {SALT[i]=rng.getByte()}
        var H=HMAC.SPhashit(HMAC.MC_SHA2,sha,m)
        let mask=(0xff as UInt8)>>(8*emlen-embits)

        if emlen < hlen + hlen +  2  {
            return [UInt8]()
        }
        var MD=[UInt8](repeating: 0,count: 8+hlen+hlen)
        for i in 0 ..< 8 {
            MD[i]=0
        }
        for i in 0 ..< hlen {
            MD[8+i]=H[i];
        }
        for i in 0 ..< hlen {
            MD[8+hlen+i]=SALT[i]
        }
        H=HMAC.SPhashit(HMAC.MC_SHA2,sha,MD)
        var f=[UInt8](repeating: 0,count: RFS)
        for i in 0 ..< emlen-hlen-hlen-2 {
            f[i]=0
        }
        f[emlen-hlen-hlen-2]=0x01
        for i in 0 ..< hlen {
            f[emlen+i-hlen-hlen-1]=SALT[i]
        }
        HMAC.MGF1XOR(sha,H,emlen-hlen-1,&f);
        f[0]&=mask;
        for i in 0 ..< hlen {
            f[emlen+i-hlen-1]=H[i]
        }
        f[emlen-1]=0xbc as UInt8
        return f
    }

    static public func PSS_VERIFY(_ sha:Int,_ m:[UInt8],_ f:[UInt8]) -> Bool
    {
        let emlen=f.count
        let embits=8*emlen-1
        let hlen=sha
        var SALT=[UInt8](repeating: 0,count: hlen)
        let mask=(0xff as UInt8)>>(8*emlen-embits)

        var HMASK=HMAC.SPhashit(HMAC.MC_SHA2,sha,m)
        if emlen < hlen + hlen +  2  {
            return false
        }
        if f[emlen-1] != 0xbc as UInt8 {
            return false
        }
        if f[0]&(~mask) != 0 {
            return false
        }
        var DB=[UInt8](repeating: 0,count: emlen-hlen-1)
        for i in 0 ..< emlen-hlen-1 {
            DB[i]=f[i]
        }
        var H=[UInt8](repeating: 0,count: hlen)
        for i in 0 ..< hlen {
            H[i]=f[emlen+i-hlen-1]
        }

        HMAC.MGF1XOR(sha,H,emlen-hlen-1,&DB)
        DB[0] &= mask

        var k = 0 as UInt8
        for i in 0 ..< emlen-hlen-hlen-2 {
            k |= DB[i]
        }
        if k != 0 {
            return false
        }
        if DB[emlen-hlen-hlen-2] != 0x01 {
            return false
        }
        for i in 0 ..< hlen {
            SALT[i]=DB[emlen+i-hlen-hlen-1]
        }
        var MD=[UInt8](repeating: 0,count: 8+hlen+hlen)
        for i in 0 ..< 8 {
            MD[i]=0
        }
        for i in 0 ..< hlen {
            MD[8+i]=HMASK[i]
        }
        for i in 0 ..< hlen {
            MD[8+hlen+i]=SALT[i]
        } 
        HMASK=HMAC.SPhashit(HMAC.MC_SHA2,sha,MD)  
        
        k=0
        for i in 0 ..< hlen {
            k |= (H[i]-HMASK[i])
        }
        if k != 0 {
            return false;
        }
        return true
    }

    /* OAEP Message Encoding for Encryption */
    static public func OAEP_ENCODE(_ sha:Int,_ m:[UInt8],_ rng: inout RAND,_ p:[UInt8]?,_ RFS:Int) -> [UInt8]
    {
        let olen=RFS-1
        let mlen=m.count
        var f=[UInt8](repeating: 0,count: RFS)

        let hlen=sha
        var SEED=[UInt8](repeating: 0,count: hlen)
        let seedlen=hlen
        if (mlen>olen-hlen-seedlen-1) {return [UInt8]()}

        var DBMASK=[UInt8](repeating: 0,count: olen-seedlen)

        let h=HMAC.SPhashit(HMAC.MC_SHA2,sha,p)

        for i in 0 ..< hlen {f[i]=h[i]}

        let slen=olen-mlen-hlen-seedlen-1

        for i in 0 ..< slen {f[hlen+i]=0}
        f[hlen+slen]=1
        for i in 0 ..< mlen {f[hlen+slen+1+i]=m[i]}

        for i in 0 ..< seedlen {SEED[i]=rng.getByte()}
        HMAC.MGF1(sha,SEED,olen-seedlen,&DBMASK)

        for i in 0 ..< olen-seedlen {DBMASK[i]^=f[i]}
        HMAC.MGF1(sha,DBMASK,seedlen,&f)

        for i in 0 ..< seedlen {f[i]^=SEED[i]}

        for i in 0 ..< olen-seedlen {f[i+seedlen]=DBMASK[i]}

    /* pad to length RFS */
        let d:Int=1;
        for i in (d...RFS-1).reversed()
            {f[i]=f[i-d]}
        for i in 0...d-1
            {f[i]=0}

        return f;
    }

    /* OAEP Message Decoding for Decryption */
    static public func OAEP_DECODE(_ sha: Int,_ p: [UInt8]?,_ f:inout [UInt8],_ RFS:Int) -> [UInt8]
    {
        let olen=RFS-1
        var k:Int
        let hlen=sha
        var SEED=[UInt8](repeating: 0,count: hlen)
        let seedlen=hlen
        var CHASH=[UInt8](repeating: 0,count: hlen)

        if olen<seedlen+hlen+1 {return [UInt8()]}
        var DBMASK=[UInt8](repeating: 0,count: olen-seedlen)
        for i in 0 ..< olen-seedlen {DBMASK[i]=0}

        if (f.count<RFS)
        {
            let d=RFS-f.count;
            for i in (d...RFS-1).reversed()
                {f[i]=f[i-d]}
            for i in 0...d-1
                {f[i]=0}

        }

        let h=HMAC.SPhashit(HMAC.MC_SHA2,sha,p)

        for i in 0 ..< hlen {CHASH[i]=h[i]}

        let x=f[0];

        for i in seedlen ..< olen
            {DBMASK[i-seedlen]=f[i+1]}

        HMAC.MGF1(sha,DBMASK,seedlen,&SEED);
        for i in 0 ..< seedlen {SEED[i]^=f[i+1]}
        HMAC.MGF1(sha,SEED,olen-seedlen,&f);
        for i in 0 ..< olen-seedlen {DBMASK[i]^=f[i]}

        var comp=true;
        for i in 0 ..< hlen
        {
            if (CHASH[i] != DBMASK[i]) {comp=false}
        }

        for i in 0 ..< olen-seedlen-hlen
        {DBMASK[i]=DBMASK[i+hlen]}

        for i in 0 ..< hlen
            {SEED[i]=0;CHASH[i]=0;}

        k=0
        while (true)
        {
            if (k>=olen-seedlen-hlen) {return [UInt8]()}
            if (DBMASK[k] != 0) {break}
            k+=1
        }

        let t=DBMASK[k];
        if (!comp || x != 0 || t != 0x01)
        {
            for i in 0 ..< olen-seedlen {DBMASK[i]=0}
            return [UInt8]()
        }

        var r=[UInt8](repeating: 0,count: olen-seedlen-hlen-k-1)

        for i in 0 ..< olen-seedlen-hlen-k-1
            {r[i]=DBMASK[i+k+1]}

        for i in 0 ..< olen-seedlen {DBMASK[i]=0}

        return r;
    }
}

/*


    let msg=String("abc");
    let MSG=[UInt8]( (msg).utf8 )
    let dst=String("P256_XMD:SHA-256_SSWU_RO_TESTGEN");
    let DST=[UInt8]( (dst).utf8 )

    var OKM=HMAC.XOF_Expand(SHA3.SHAKE128,48,DST,MSG)
    print("OKM= ",terminator:""); printBinary(OKM)
    OKM=HMAC.XMD_Expand(HMAC.MC_SHA2,32,48,DST,MSG)
    print("OKM= ",terminator:""); printBinary(OKM)



    var ikm=[UInt8] (repeating:0,count:22)
    var salt=[UInt8] (repeating:0,count:13)
    var info=[UInt8] (repeating:0,count:10)
    var prk=[UInt8] ()
    var okm=[UInt8] ()

    for i in 0..<22 {ikm[i]=0x0b}
    for i in 0..<13 {salt[i]=UInt8(i)}
    for i in 0..<10 {info[i]=UInt8(0xf0+i)}

    prk=HMAC.HKDF_Extract(HMAC.MC_SHA2,32,salt,ikm)

    print("PRK= ")
    for i in 0..<prk.count {
        let h=String(prk[i],radix:16)
        print("\(h)", terminator: "")
    }
    print("")

    okm=HMAC.HKDF_Expand(HMAC.MC_SHA2,32,42,prk,info)
    print("OKM= ")
    for i in 0..<okm.count {
        let h=String(okm[i],radix:16)
        print("\(h)", terminator: "")
    }
    print("")

*/
