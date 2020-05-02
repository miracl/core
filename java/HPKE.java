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

/* Hybrid Public Key Encryption */

/* Following https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/?include_text=1 */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HMAC;
import org.miracl.core.GCM;

public final class HPKE {

    static void reverse(byte[] X) {
	    int lx=X.length;
	    for (int i=0;i<lx/2;i++)  {
		    byte ch =X[i];
		    X[i]=X[lx-i-1];
		    X[lx-i-1]=ch;
	    }
    }
    
    public static byte[] encap(int config_id,RAND RNG,byte[] SKE,byte[] pkE,byte[] pkR) {
        byte[] skE = new byte[ECDH.EGS];
        byte[] Z = new byte[ECDH.EFS];
	    int kem = config_id&7;
	    if (RNG == null) {
		    for (int i=0;i<ECDH.EGS;i++) 
			    skE[i]=SKE[i];
		    if (kem==2 || kem==4) {
			    reverse(skE);
			    if (kem==2) {
				    skE[ECDH.EGS-1]&=248;
				    skE[0]&=127;
				    skE[0]|=64;
			    } else {
				    skE[ECDH.EGS-1]&=252;
				    skE[0]|=128;
			    }
		    }
	    }
	    ECDH.KEY_PAIR_GENERATE(RNG, skE, pkE);
	    if (kem==2 || kem==4) 
		    reverse(pkR);
	    ECDH.SVDP_DH(skE, pkR, Z);
	    if (kem==2 || kem==4) {
		    reverse(pkR);
		    reverse(pkE);
		    reverse(Z);
	    }
    	return Z;
    }

    public static byte[] decap(int config_id,byte[] pkE,byte[] SKR) {
        byte[] skR = new byte[ECDH.EGS];
        byte[] Z = new byte[ECDH.EFS];
	    int kem = config_id&7;
	    for (int i=0;i<ECDH.EGS;i++)
		    skR[i]=SKR[i];
	    if (kem==2 || kem==4) {
		    reverse(skR);
		    reverse(pkE);
		    if (kem==2) {
			    skR[ECDH.EGS-1]&=248;
			    skR[0]&=127;
			    skR[0]|=64;
		    } else {
			    skR[ECDH.EGS-1]&=252;
			    skR[0]|=128;
		    }
	    }
	    ECDH.SVDP_DH(skR, pkE, Z);
	    if (kem==2 || kem==4) {
		    reverse(pkE);
		    reverse(Z);
	    }
	    return Z;
    }

    public static byte[] authEncap(int config_id,RAND RNG,byte[] SKE,byte[] pkE,byte[] pkR,byte[] SKI) {
        byte[] skE = new byte[ECDH.EGS];
        byte[] Z = new byte[ECDH.EFS];
        byte[] skI = new byte[ECDH.EGS];
        byte[] NZ = new byte[ECDH.EFS];
	
	    int kem = config_id&7;

	    if (RNG == null) {
		    for (int i=0;i<ECDH.EGS;i++) {
			    skE[i]=SKE[i];
			    skI[i]=SKI[i];
		    }	
		    if (kem==2 || kem==4) {
			    reverse(skE);
			    reverse(skI);
			    if (kem==2) {
				    skE[ECDH.EGS-1]&=248;
				    skE[0]&=127;
				    skE[0]|=64;
				    skI[ECDH.EGS-1]&=248;
				    skI[0]&=127;
				    skI[0]|=64;	
			    } else {
				    skE[ECDH.EGS-1]&=252;
				    skE[0]|=128;
				    skI[ECDH.EGS-1]&=252;
				    skI[0]|=128;
			    }
		    }
	    }
	    ECDH.KEY_PAIR_GENERATE(RNG, skE, pkE);
	    if (kem==2 || kem==4) 
		    reverse(pkR);
	
	    ECDH.SVDP_DH(skE, pkR, Z);
	    ECDH.SVDP_DH(skI, pkR, NZ);
	    if (kem==2 || kem==4) {
		    reverse(pkR);
		    reverse(pkE);
		    reverse(Z);
		    reverse(NZ);
	    }
	    byte[] ZZ = new byte[2*ECDH.EFS];
	    for (int i=0;i<ECDH.EFS;i++) {
		    ZZ[i] = Z[i];
            ZZ[ECDH.EFS+i]= NZ[i];
	    }
	    return ZZ;
    }

    public static byte[] authDecap(int config_id,byte[] pkE,byte[] SKR,byte[] pkI) {
        byte[] skR = new byte[ECDH.EGS];
        byte[] Z = new byte[ECDH.EFS];
        byte[] NZ = new byte[ECDH.EFS];
		int kem = config_id&7;

	    for (int i=0;i<ECDH.EGS;i++) 
		skR[i]=SKR[i];
	
	    if (kem==2 || kem==4) {
		    reverse(skR);
		    reverse(pkE);
		    reverse(pkI);
		    if (kem==2) {
			    skR[ECDH.EGS-1]&=248;
			    skR[0]&=127;
			    skR[0]|=64;
		    } else {
		    	skR[ECDH.EGS-1]&=252;
			    skR[0]|=128;
		    }  
	    }
	    ECDH.SVDP_DH(skR, pkE, Z);
	    ECDH.SVDP_DH(skR, pkI, NZ);
	    if (kem==2 || kem==4) {
		    reverse(pkI);
		    reverse(pkE);
		    reverse(Z);
		    reverse(NZ);
	    }
	    byte[] ZZ = new byte[2*ECDH.EFS];
	    for (int i=0;i<ECDH.EFS;i++) {
		    ZZ[i] = Z[i];
            ZZ[ECDH.EFS+i]= NZ[i];
	    }
	    return ZZ;
    }

    public static void keySchedule(int config_id,byte[] key,byte[] nonce,int mode,byte[] pkR,byte[] Z,byte[] pkE,byte[] info,byte[] psk,byte[] pskID,byte[] pkI) {
        int ctxlen=7+3*pkR.length+2*CONFIG_CURVE.HASH_TYPE;
	    byte[] context = new byte[ctxlen];
	    int kem_id=config_id&7;
	    int kdf_id=(config_id>>3)&3;
	    int aead_id=(config_id>>5)&3;
        int k=0;

	    byte[] ar = HMAC.inttoBytes(mode, 1);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];
	
	    ar=HMAC.inttoBytes(kem_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    ar=HMAC.inttoBytes(kdf_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    ar=HMAC.inttoBytes(aead_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    for (int i=0;i<pkE.length;i++) 
		    context[k++] = pkE[i];
	
	    for (int i=0;i<pkR.length;i++) 
		    context[k++] = pkR[i];

	    if (pkI==null) {
		    for (int i=0;i<pkR.length;i++)
			    context[k++] = 0;
	    } else {
		    for (int i=0;i<pkI.length;i++)
			    context[k++] = pkI[i];
	    }
	    byte[] H=HMAC.SPhashit(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,pskID);
	    for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++) 
		    context[k++] = H[i];
	
	    H=HMAC.SPhashit(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,info);
	    for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++) 
		    context[k++] = H[i];

	    byte[] secret=HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,psk,Z);
	
	    String pp = new String("hpke key");
	    ar = pp.getBytes(); k=0;
        byte[] full_context = new byte[ctxlen+ar.length]; 
	    for (int i=0;i<ar.length;i++)
		    full_context[k++] = ar[i];
	    for (int i=0;i<ctxlen;i++)
		    full_context[k++] = context[i];
	
	    byte[] ex=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,CONFIG_CURVE.AESKEY,secret,full_context);
        for (int i=0;i<ex.length;i++) key[i]=ex[i];

	    pp = new String("hpke nonce");
	    ar = pp.getBytes(); k=0;
        full_context = new byte[ctxlen+ar.length]; 
	    for (int i=0;i<ar.length;i++)
		    full_context[k++] = ar[i];
	    for (int i=0;i<ctxlen;i++)
		    full_context[k++] = context[i];
	
	    ex=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,12,secret,full_context);
        for (int i=0;i<ex.length;i++) nonce[i]=ex[i];
    }
}


