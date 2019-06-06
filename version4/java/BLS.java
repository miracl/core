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

/* Boneh-Lynn-Shacham signature API Functions */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.SHA3;

public class BLS
{
	public static final int BFS=CONFIG_BIG.MODBYTES;
	public static final int BGS=CONFIG_BIG.MODBYTES;
	public static final int BLS_OK=0;
	public static final int BLS_FAIL=-1;

	public static FP4[] G2_TAB;


/* hash a message to an ECP point, using SHA3 */

	static ECP bls_hashit(String m)
	{
		SHA3 sh=new SHA3(SHA3.SHAKE256);
		byte[] hm=new byte[BFS];
		byte[] t=m.getBytes();
		for (int i=0;i<t.length;i++)
			sh.process(t[i]);
		sh.shake(hm,BFS);    
		ECP P=ECP.mapit(hm);
		return P;
	}

	public static void init()
	{
		ECP2 G=ECP2.generator();
		G2_TAB=PAIR.precomp(G);
	}

/* generate key pair, private key S, public key W */

	public static int KeyPairGenerate(RAND RNG,byte[] S,byte[] W)
	{
		ECP2 G=ECP2.generator();
		BIG q=new BIG(ROM.CURVE_Order);
		BIG s=BIG.randtrunc(q,16*CONFIG_CURVE.AESKEY,RNG);
		s.toBytes(S);
		G=PAIR.G2mul(G,s);
		G.toBytes(W);
		return BLS_OK;
	}

/* Sign message m using private key S to produce signature SIG */

	public static int sign(byte[] SIG,String m,byte[] S)
	{
		ECP D=bls_hashit(m);
		BIG s=BIG.fromBytes(S);
		D=PAIR.G1mul(D,s);
		D.toBytes(SIG,true);
		return BLS_OK;
	}

/* Verify signature given message m, the signature SIG, and the public key W */

	public static int verify(byte[] SIG,String m,byte[] W)
	{
		ECP HM=bls_hashit(m);
		ECP D=ECP.fromBytes(SIG);
		ECP2 PK=ECP2.fromBytes(W);
		D.neg();

// Use new multi-pairing mechanism 
		FP12[] r=PAIR.initmp();
		PAIR.another_pc(r,G2_TAB,D);
		PAIR.another(r,PK,HM);
		FP12 v=PAIR.miller(r);

//.. or alternatively
//		ECP2 G=ECP2.generator();
//		FP12 v=PAIR.ate2(G,D,PK,HM);

		v=PAIR.fexp(v);
		if (v.isunity())
			return BLS_OK;
		return BLS_FAIL;
	}
}
