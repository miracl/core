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
//  rsa.swift
//
//  Created by Michael Scott on 25/06/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

import core

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

    static public let HASH_TYPE=HMAC.SHA256

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

