/*
	Licensed to the Apache Software Foundation (ASF) under one
	or more contributor license agreements.  See the NOTICE file
	distributed with this work for additional information
	regarding copyright ownership.  The ASF licenses this file
	to you under the Apache License, Version 2.0 (the
	"License"); you may not use this file except in compliance
	with the License.  You may obtain a copy of the License at
	
	http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing,
	software distributed under the License is distributed on an
	"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
	KIND, either express or implied.  See the License for the
	specific language governing permissions and limitations
	under the License.
*/

//
//  gcm.swift
//
//  Created by Michael Scott on 23/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

import Foundation

/*
* Implementation of the AES-GCM Encryption/Authentication
*
* Some restrictions..
* 1. Only for use with AES
* 2. Returned tag is always 128-bits. Truncate at your own risk.
* 3. The order of function calls must follow some rules
*
* Typical sequence of calls..
* 1. call GCM_init
* 2. call GCM_add_header any number of times, as long as length of header is multiple of 16 bytes (block size)
* 3. call GCM_add_header one last time with any length of header
* 4. call GCM_add_cipher any number of times, as long as length of cipher/plaintext is multiple of 16 bytes
* 5. call GCM_add_cipher one last time with any length of cipher/plaintext
* 6. call GCM_finish to extract the tag.
*
* See http://www.mindspring.com/~dmcgrew/gcm-nist-6.pdf
*/

struct GCM {
    static let NB:Int=4
    static let GCM_ACCEPTING_HEADER:Int=0
    static let GCM_ACCEPTING_CIPHER:Int=1
    static let GCM_NOT_ACCEPTING_MORE:Int=2
    static let GCM_FINISHED:Int=3
    static let GCM_ENCRYPTING:Int=0
    static let GCM_DECRYPTING:Int=1
    
    private var table=[[UInt32]](repeating: [UInt32](repeating: 0,count: 4),count: 128) /* 2k bytes */
    private var stateX=[UInt8](repeating: 0,count: 16)
    private var Y_0=[UInt8](repeating: 0,count: 16)
    
    private var counter:Int=0
    private var lenA=[UInt32](repeating: 0,count: 2)
    private var lenC=[UInt32](repeating: 0,count: 2)
    private var status:Int=0
    private var a=AES()
    
    private static func pack(_ b: [UInt8]) -> UInt32
    { /* pack bytes into a 32-bit Word */
        var r=((UInt32(b[0])&0xff)<<24)|((UInt32(b[1])&0xff)<<16)
        r = r|((UInt32(b[2])&0xff)<<8)|(UInt32(b[3])&0xff)
        return r
    }
    
    private static func unpack(_ a: UInt32) -> [UInt8]
    { /* unpack bytes from a word */
        let b:[UInt8]=[UInt8((a>>24)&0xff),UInt8((a>>16)&0xff),UInt8((a>>8)&0xff),UInt8(a&0xff)];
        return b
    }
    
    private mutating func precompute(_ H: [UInt8])
    {
        var b=[UInt8](repeating: 0,count: 4)
        var j=0
        for i in 0 ..< GCM.NB
        {
            b[0]=H[j]; b[1]=H[j+1]; b[2]=H[j+2]; b[3]=H[j+3];
            table[0][i]=GCM.pack(b);
            j+=4
        }
        for i in 1 ..< 128
        {
            var c:UInt32=0
            for j in 0 ..< GCM.NB {table[i][j]=c|(table[i-1][j])>>1; c=table[i-1][j]<<31;}
            if c != 0  {table[i][0]^=0xE1000000} /* irreducible polynomial */
        }
    }
 
    private mutating func gf2mul()
    { /* gf2m mul - Z=H*X mod 2^128 */
        var P=[UInt32](repeating: 0,count: 4)
    
        for i in 0 ..< 4 {P[i]=0}
        var j=8; var m=0;
        for i in 0 ..< 128
        {
            j-=1
            var c=UInt32((stateX[m]>>UInt8(j))&1); c = ( ~c ) + 1
            for k in 0 ..< GCM.NB {P[k]^=(table[i][k] & c)}
            if (j==0)
            {
		j=8; m += 1;
                if (m==16) {break}
            }
        }
        j=0
        for i in 0 ..< GCM.NB
        {
            var b=GCM.unpack(P[i])
            stateX[j]=b[0]; stateX[j+1]=b[1]; stateX[j+2]=b[2]; stateX[j+3]=b[3];
            j+=4
        }
    }
    private mutating func wrap()
    { /* Finish off GHASH */
        var F=[UInt32](repeating: 0,count: 4)
        var L=[UInt8](repeating: 0,count: 16)
    
    /* convert lengths from bytes to bits */
        F[0]=(lenA[0]<<3)|(lenA[1]&0xE0000000)>>29
        F[1]=lenA[1]<<3;
        F[2]=(lenC[0]<<3)|(lenC[1]&0xE0000000)>>29
        F[3]=lenC[1]<<3;
        var j=0
        for i in 0 ..< GCM.NB
        {
            var b=GCM.unpack(F[i]);
            L[j]=b[0]; L[j+1]=b[1]; L[j+2]=b[2]; L[j+3]=b[3]
            j+=4
        }
        for i in 0 ..< 16 {stateX[i]^=L[i]}
        gf2mul()
    }
    
    private mutating func ghash(_ plain: [UInt8],_ len: Int) -> Bool
    {
    //    var B=[UInt8](count:16,repeatedValue:0)
    
        if status==GCM.GCM_ACCEPTING_HEADER {status=GCM.GCM_ACCEPTING_CIPHER}
        if (status != GCM.GCM_ACCEPTING_CIPHER) {return false}
        
        var j=0;
        while (j<len)
        {
            for i in 0 ..< 16
            {
				stateX[i]^=plain[j];
                j+=1;
                lenC[1]+=1; if lenC[1]==0 {lenC[0]+=1}
                if j>=len {break;}
            }
            gf2mul();
        }
        if len%16 != 0 {status=GCM.GCM_NOT_ACCEPTING_MORE}
        return true;
    }
    
    /* Initialize GCM mode */
    mutating func init_it(_ nk: Int,_ key: [UInt8],_ niv: Int,_ iv: [UInt8])
    { /* iv size niv is usually 12 bytes (96 bits). AES key size nk can be 16,24 or 32 bytes */
        var H=[UInt8](repeating: 0,count: 16)
        
        for i in 0 ..< 16 {H[i]=0; stateX[i]=0}
        
        a.init_it(AES.ECB,nk,key,iv)
        a.ecb_encrypt(&H);    /* E(K,0) */
        precompute(H)
        
        lenA[0]=0;lenC[0]=0;lenA[1]=0;lenC[1]=0;
        if (niv==12)
        {
            for i in 0 ..< 12 {a.f[i]=iv[i]}
            var b=GCM.unpack(UInt32(1))
            a.f[12]=b[0]; a.f[13]=b[1]; a.f[14]=b[2]; a.f[15]=b[3];  /* initialise IV */
            for i in 0 ..< 16 {Y_0[i]=a.f[i]}
        }
        else
        {
            status=GCM.GCM_ACCEPTING_CIPHER;
            _ = ghash(iv,niv) /* GHASH(H,0,IV) */
            wrap()
            for i in 0 ..< 16 {a.f[i]=stateX[i];Y_0[i]=a.f[i];stateX[i]=0}
            lenA[0]=0;lenC[0]=0;lenA[1]=0;lenC[1]=0;
        }
        status=GCM.GCM_ACCEPTING_HEADER;
    }
    
    /* Add Header data - included but not encrypted */
    mutating func add_header(_ header: [UInt8],_ len: Int) -> Bool
    { /* Add some header. Won't be encrypted, but will be authenticated. len is length of header */
        if status != GCM.GCM_ACCEPTING_HEADER {return false}
    
        var j=0
        while (j<len)
        {
            for i in 0 ..< 16
            {
				stateX[i]^=header[j];
                j+=1;
                lenA[1]+=1; if lenA[1]==0 {lenA[0]+=1}
                if j>=len {break}
            }
            gf2mul();
        }
        if len%16 != 0 {status=GCM.GCM_ACCEPTING_CIPHER}
        return true;
    }
    /* Add Plaintext - included and encrypted */
    mutating func add_plain(_ plain: [UInt8],_ len: Int) -> [UInt8]
    {
        var B=[UInt8](repeating: 0,count: 16)
        var b=[UInt8](repeating: 0,count: 4)
        
        var cipher=[UInt8](repeating: 0,count: len)
        var counter:UInt32=0
        if status == GCM.GCM_ACCEPTING_HEADER {status=GCM.GCM_ACCEPTING_CIPHER}
        if status != GCM.GCM_ACCEPTING_CIPHER {return [UInt8]()}
        
        var j=0
        while (j<len)
        {
    
            b[0]=a.f[12]; b[1]=a.f[13]; b[2]=a.f[14]; b[3]=a.f[15];
            counter=GCM.pack(b);
            counter+=1;
            b=GCM.unpack(counter);
            a.f[12]=b[0]; a.f[13]=b[1]; a.f[14]=b[2]; a.f[15]=b[3]; /* increment counter */
            for i in 0 ..< 16 {B[i]=a.f[i]}
            a.ecb_encrypt(&B);        /* encrypt it  */
    
            for i in 0 ..< 16
            {
		cipher[j]=(plain[j]^B[i]);
		stateX[i]^=cipher[j];
                j+=1;
                lenC[1]+=1; if lenC[1]==0 {lenC[0]+=1}
                if j>=len {break}
            }
            gf2mul();
        }
        if len%16 != 0 {status=GCM.GCM_NOT_ACCEPTING_MORE}
        return cipher;
    }
    /* Add Ciphertext - decrypts to plaintext */
    mutating func add_cipher(_ cipher: [UInt8],_ len: Int) -> [UInt8]
    {
        var B=[UInt8](repeating: 0,count: 16)
        var b=[UInt8](repeating: 0,count: 4)
        
        var plain=[UInt8](repeating: 0,count: len)
        var counter:UInt32=0
        
        if status==GCM.GCM_ACCEPTING_HEADER {status=GCM.GCM_ACCEPTING_CIPHER}
        if status != GCM.GCM_ACCEPTING_CIPHER {return [UInt8]()}
    
        var j=0
        while (j<len)
        {
    
            b[0]=a.f[12]; b[1]=a.f[13]; b[2]=a.f[14]; b[3]=a.f[15];
            counter=GCM.pack(b);
            counter+=1;
            b=GCM.unpack(counter);
            a.f[12]=b[0]; a.f[13]=b[1]; a.f[14]=b[2]; a.f[15]=b[3]; /* increment counter */
            for i in 0 ..< 16 {B[i]=a.f[i]}
            a.ecb_encrypt(&B);        /* encrypt it  */
            for i in 0 ..< 16
            {
                let oc=cipher[j]
                plain[j]=(cipher[j]^B[i]);
                stateX[i]^=oc;
                j+=1;
                lenC[1]+=1; if lenC[1]==0 {lenC[0]+=1}
                if j>=len {break}
            }
            gf2mul()
        }
        if len%16 != 0 {status=GCM.GCM_NOT_ACCEPTING_MORE}
        return plain;
    }
    
    /* Finish and extract Tag */
    mutating func finish(_ extract: Bool) -> [UInt8]
    { /* Finish off GHASH and extract tag (MAC) */
        var tag=[UInt8](repeating: 0,count: 16)
    
        wrap();
        /* extract tag */
        if (extract)
        {
            a.ecb_encrypt(&Y_0);        /* E(K,Y0) */
            for i in 0 ..< 16 {Y_0[i]^=stateX[i]}
            for i in 0 ..< 16 {tag[i]=Y_0[i];Y_0[i]=0;stateX[i]=0;}
        }
        status=GCM.GCM_FINISHED;
        a.end();
        return tag;
    }
    
    static func hex2bytes(_ s: String) -> [UInt8]
    {
        var array=Array(arrayLiteral: s)
        let len=array.count;
        var data=[UInt8](repeating: 0,count: len/2)

        var i=0
        while (i<len)
        {
            data[i / 2] = UInt8(strtoul(String(array[i]),nil,16)<<4)+UInt8(strtoul(String(array[i+1]),nil,16))
            i+=2
        }
        return data;
    }
    
    
}

