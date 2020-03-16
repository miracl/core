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
        H.process(UInt8(DST.count & 0xff))
        for i in 0..<DST.count {
            H.process(DST[i])
        }
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
	    TMP[3]=UInt8(DST.count & 0xff)
	    for j in 0..<DST.count {
		    TMP[4+j]=DST[j]
	    }
	    let H0=HMAC.GPhashit(hf, hlen, 0, blk, MSG, -1, TMP)
        var H1=[UInt8](repeating: 0,count: hlen)
        var TMP2=[UInt8](repeating: 0,count: DST.count+2)

        var k=0
        for i in 1...ell {
            for j in 0..<hlen {
                H1[j]^=H0[j]
            }          
            TMP2[0]=UInt8(i)
            TMP2[1]=UInt8(DST.count & 0xff)
            for j in 0..<DST.count {
                TMP2[2+j]=DST[j]
            }

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
