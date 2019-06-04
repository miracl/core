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
//  rsa.swift
//
//  Created by Michael Scott on 25/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

import amcl

/* RSA API high-level functions  */

public struct rsa_private_key {
    var p:FF
    var q:FF
    var dp:FF
    var dq:FF
    var c:FF
    
    public init(_ n: Int)
    {
    p=FF(n);
    q=FF(n);
    dp=FF(n);
    dq=FF(n);
    c=FF(n);
    }
}

public struct rsa_public_key
{
    var e:Int
    var n:FF
    
    public init(_ m:Int)
    {
        e=0;
        n=FF(m);
    }
}

public struct RSA {
    
    static public let RFS=Int(CONFIG_BIG.MODBYTES*CONFIG_FF.FFLEN)
    static public let SHA256=32
    static public let SHA384=48
    static public let SHA512=64
    
    static public let HASH_TYPE=SHA384
    
    /* Hash number (optional) and string to array size of Bigs */
 
    static private func hashit(_ sha:Int,_ A: [UInt8]?,_ n: Int32) -> [UInt8]
    {
        if sha==RSA.SHA256
        {
            var H=HASH256()
            if A != nil {H.process_array(A!)}
            if n >= 0 {H.process_num(n)}
            let R=H.hash()
            return R
        }
        if sha==RSA.SHA384
        {
            var H=HASH384()
            if A != nil {H.process_array(A!)}
            if n >= 0 {H.process_num(n)}
            let R=H.hash()
            return R
        }
        if sha==RSA.SHA512
        {
            var H=HASH512()
            if A != nil {H.process_array(A!)}
            if n >= 0 {H.process_num(n)}
            let R=H.hash()
            return R
        }
        return [UInt8]()
    }

    /* generate an RSA key pair */
    
    static public func KEY_PAIR(_ rng: inout RAND,_ e:Int,_ PRIV: inout rsa_private_key,_ PUB: inout rsa_public_key)
    { /* IEEE1363 A16.11/A16.12 more or less */
    
        let n=PUB.n.getlen()/2;
        let t = FF(n);
        let p1=FF(n);
        let q1=FF(n);
    
        while true
        {
    
            PRIV.p.random(&rng);
            while PRIV.p.lastbits(2) != 3 {PRIV.p.inc(1)}
            while !FF.prime(PRIV.p,&rng) {PRIV.p.inc(4)}
    
            p1.copy(PRIV.p);
            p1.dec(1);
    
            if p1.cfactor(e) {continue}
            break;
        }
    
        while true
        {
            PRIV.q.random(&rng);
            while PRIV.q.lastbits(2) != 3 {PRIV.q.inc(1)}
            while !FF.prime(PRIV.q,&rng) {PRIV.q.inc(4)}
    
            q1.copy(PRIV.q);
            q1.dec(1);
    
            if q1.cfactor(e) {continue}
    
            break;
        }
    
        PUB.n=FF.mul(PRIV.p,PRIV.q);
        PUB.e=e;
    
        t.copy(p1);
        t.shr();
        PRIV.dp.set(e);
        PRIV.dp.invmodp(t);
        if (PRIV.dp.parity()==0) {PRIV.dp.add(t)}
        PRIV.dp.norm();
    
        t.copy(q1);
        t.shr();
        PRIV.dq.set(e);
        PRIV.dq.invmodp(t);
        if (PRIV.dq.parity()==0) {PRIV.dq.add(t)}
        PRIV.dq.norm();
    
        PRIV.c.copy(PRIV.p);
        PRIV.c.invmodp(PRIV.q);
    
        return;
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
            var B=RSA.hashit(sha,Z,counter)
            if (k+hlen>olen) {for i in 0 ..< olen%hlen {K[k]=B[i]; k+=1}}
            else {for i in 0 ..< hlen {K[k]=B[i]; k+=1}}
        }
    }
    
    static let SHA256ID:[UInt8]=[0x30,0x31,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x01,0x05,0x00,0x04,0x20]
    static let SHA384ID:[UInt8]=[0x30,0x41,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x02,0x05,0x00,0x04,0x30]
    static let SHA512ID:[UInt8]=[0x30,0x51,0x30,0x0d,0x06,0x09,0x60,0x86,0x48,0x01,0x65,0x03,0x04,0x02,0x03,0x05,0x00,0x04,0x40]
    
    /* PKCS 1.5 padding of a message to be signed */
    static public func PKCS15(_ sha:Int,_ m:[UInt8],_ w:inout [UInt8])->Bool
    {
        let hlen=sha
        let olen=Int(CONFIG_FF.FF_BITS/8)
        let idlen=19;
        
        if olen<idlen+hlen+10 {return false}
        let H=hashit(sha,m,-1)
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
        
        if hlen==RSA.SHA256
        {
            for j in 0 ..< idlen {w[i]=SHA256ID[j]; i+=1}
        }
        if hlen==RSA.SHA384
        {
            for j in 0 ..< idlen {w[i]=SHA384ID[j]; i+=1}
        }
        if hlen==RSA.SHA512
        {
            for j in 0 ..< idlen {w[i]=SHA512ID[j]; i+=1}
        }
        
        for j in 0 ..< hlen {w[i]=H[j];i+=1}
        
        return true
    }
    
    
    /* OAEP Message Encoding for Encryption */
    static public func OAEP_ENCODE(_ sha:Int,_ m:[UInt8],_ rng: inout RAND,_ p:[UInt8]?) -> [UInt8]
    {
        let olen=RFS-1;
        let mlen=m.count;
        var f=[UInt8](repeating: 0,count: RSA.RFS)
    
        let hlen=sha;
        var SEED=[UInt8](repeating: 0,count: hlen)
        let seedlen=hlen;
        if (mlen>olen-hlen-seedlen-1) {return [UInt8]()}
    
        var DBMASK=[UInt8](repeating: 0,count: olen-seedlen)
        
        var h=hashit(sha,p,-1)
        for i in 0 ..< hlen {f[i]=h[i]}
    
        let slen=olen-mlen-hlen-seedlen-1;
    
        for i in 0 ..< slen {f[hlen+i]=0}
        f[hlen+slen]=1;
        for i in 0 ..< mlen {f[hlen+slen+1+i]=m[i]}
    
        for i in 0 ..< seedlen {SEED[i]=rng.getByte()}
        RSA.MGF1(sha,SEED,olen-seedlen,&DBMASK)
    
        for i in 0 ..< olen-seedlen {DBMASK[i]^=f[i]}
        RSA.MGF1(sha,DBMASK,seedlen,&f)
    
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
    static public func OAEP_DECODE(_ sha: Int,_ p: [UInt8]?,_ f:inout [UInt8]) -> [UInt8]
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
    
        var h=hashit(sha,p,-1)
        for i in 0 ..< hlen {CHASH[i]=h[i]}
    
        let x=f[0];
    
        for i in seedlen ..< olen
            {DBMASK[i-seedlen]=f[i+1]}
    
        RSA.MGF1(sha,DBMASK,seedlen,&SEED);
        for i in 0 ..< seedlen {SEED[i]^=f[i+1]}
        RSA.MGF1(sha,SEED,olen-seedlen,&f);
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
    /* destroy the Private Key structure */
    static public func PRIVATE_KEY_KILL(_ PRIV: rsa_private_key)
    {
        PRIV.p.zero();
        PRIV.q.zero();
        PRIV.dp.zero();
        PRIV.dq.zero();
        PRIV.c.zero();
    }
    /* RSA encryption with the public key */
    static public func ENCRYPT(_ PUB: rsa_public_key,_ F:[UInt8],_ G:inout [UInt8])
    {
        let n=PUB.n.getlen()
        let f=FF(n)
    
        FF.fromBytes(f,F)
        f.power(PUB.e,PUB.n)
        f.toBytes(&G)
    }
    /* RSA decryption with the private key */
    static public func DECRYPT(_ PRIV: rsa_private_key,_ G:[UInt8],_ F:inout [UInt8])
    {
        let n=PRIV.p.getlen()
        let g=FF(2*n)
    
        FF.fromBytes(g,G)
        let jp=g.dmod(PRIV.p)
        var jq=g.dmod(PRIV.q)
    
        jp.skpow(PRIV.dp,PRIV.p)
        jq.skpow(PRIV.dq,PRIV.q)
    
        g.zero()
        g.dscopy(jp)
        jp.mod(PRIV.q)
        if (FF.comp(jp,jq)>0) {jq.add(PRIV.q)}
        jq.sub(jp)
        jq.norm()
    
        var t=FF.mul(PRIV.c,jq)
        jq=t.dmod(PRIV.q)
    
        t=FF.mul(jq,PRIV.p)
        g.add(t);
        g.norm();
    
        g.toBytes(&F);
    }
    
}

