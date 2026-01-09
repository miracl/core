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

/* RSA API high-level functions  */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HMAC;

public final class RSA {

	public static final int RFS=CONFIG_BIG.MODBYTES*CONFIG_FF.FFLEN;
	public static final int SHA256=32;
	public static final int SHA384=48;
	public static final int SHA512=64;

	public static final int HASH_TYPE=SHA256;

/* Hash number (optional) and string to array size of Bigs */

/* generate an RSA key pair */

// Input private key from OpenSSL format
// e.g as in openssl rsa -in privkey.pem -noout -text
// Note order swap - For MIRACL c=1/p mod q, for OpenSSL c=1/q mod p
    public static void PRIVATE_KEY_FROM_OPENSSL(byte[] P,byte[] Q,byte[] DP,byte[] DQ,byte[] C,private_key PRIV)
    {
        FF.fromBytes(PRIV.p,Q);
        FF.fromBytes(PRIV.q,P);
        FF.fromBytes(PRIV.dp,DQ);
        FF.fromBytes(PRIV.dq,DP);
        FF.fromBytes(PRIV.c,C);
    }

    public static void KEY_PAIR_FROM_OPENSSL(int e,byte[] P,byte[] Q,byte[] DP,byte[] DQ,byte[] C,private_key PRIV,public_key PUB)
    {
        PRIVATE_KEY_FROM_OPENSSL(P,Q,DP,DQ,C,PRIV);
		PUB.n=FF.mul(PRIV.p,PRIV.q);
		PUB.e=e;
    }

	public static void KEY_PAIR(RAND rng,int e,private_key PRIV,public_key PUB)
	{ /* IEEE1363 A16.11/A16.12 more or less */

		int n=PUB.n.getlen()/2;
		FF t = new FF(n);
		FF p1=new FF(n);
		FF q1=new FF(n);

        for (;;)
        {
            for (;;)
            {
                PRIV.p.random(rng);
                while (PRIV.p.lastbits(2)!=3) PRIV.p.inc(1);
                while (!FF.prime(PRIV.p,rng)) PRIV.p.inc(4);

                p1.copy(PRIV.p);
                p1.dec(1);

                if (p1.cfactor(e)) continue;
                break;
            }

            for (;;)
            {
                PRIV.q.random(rng);
                while (PRIV.q.lastbits(2)!=3) PRIV.q.inc(1);
                while (!FF.prime(PRIV.q,rng)) PRIV.q.inc(4);
                
                q1.copy(PRIV.q);
                q1.dec(1);

                if (q1.cfactor(e)) continue;

                break;
            }

            PUB.n=FF.mul(PRIV.p,PRIV.q);
            if (PUB.n.topbit()==1) break;
        }
		PUB.e=e;

		t.copy(p1);
		t.shr();
		PRIV.dp.set(e);
		PRIV.dp.invmodp(t);
		if (PRIV.dp.parity()==0) PRIV.dp.add(t);
		PRIV.dp.norm();

		t.copy(q1);
		t.shr();
		PRIV.dq.set(e);
		PRIV.dq.invmodp(t);
		if (PRIV.dq.parity()==0) PRIV.dq.add(t);
		PRIV.dq.norm();

		PRIV.c.copy(PRIV.p);
		PRIV.c.invmodp(PRIV.q);

		return;
	}

	/* destroy the Private Key structure */
	public static void PRIVATE_KEY_KILL(private_key PRIV)
	{
		PRIV.p.zero();
		PRIV.q.zero();
		PRIV.dp.zero();
		PRIV.dq.zero();
		PRIV.c.zero();
	}

	/* RSA encryption with the public key */
	public static void ENCRYPT(public_key PUB,byte[] F,byte[] G)
	{
		int n=PUB.n.getlen();
		FF f=new FF(n);
		FF.fromBytes(f,F);
		f.power(PUB.e,PUB.n);
		f.toBytes(G);
	}

	/* RSA decryption with the private key */
	public static void DECRYPT(private_key PRIV,byte[] G,byte[] F)
	{
		int n=PRIV.p.getlen();
		FF g=new FF(2*n);

		FF.fromBytes(g,G);
		FF jp=g.dmod(PRIV.p);
		FF jq=g.dmod(PRIV.q);

		jp.skpow(PRIV.dp,PRIV.p);
		jq.skpow(PRIV.dq,PRIV.q);

		g.zero();
		g.dscopy(jp);
		jp.mod(PRIV.q);
		if (FF.comp(jp,jq)>0) jq.add(PRIV.q);
		jq.sub(jp);
		jq.norm();

		FF t=FF.mul(PRIV.c,jq);
		jq=t.dmod(PRIV.q);

		t=FF.mul(jq,PRIV.p);
		g.add(t);
		g.norm();

		g.toBytes(F);
	}
}
