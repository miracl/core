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

    static final int GROUP = ECDH.EGS;
    static final int FIELD = ECDH.EFS;

/*
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }
*/
    static void reverse(byte[] X) {
	    int lx=X.length;
	    for (int i=0;i<lx/2;i++)  {
		    byte ch =X[i];
		    X[i]=X[lx-i-1];
		    X[lx-i-1]=ch;
	    }
    }

    static byte[] LabeledExtract(byte[] SALT,String label,byte[] IKM) {
        String rfc="RFCXX"+"XX ";
        byte[] prefix=(rfc+label).getBytes();
        int IKMlen;
        if (IKM==null)
            IKMlen=0;
        else
            IKMlen=IKM.length;
        byte[] LIKM = new byte[prefix.length+IKMlen];
        int k=0;
        for (int i=0;i<prefix.length;i++)
            LIKM[k++]=prefix[i];
        if (IKM!=null) {
            for (int i=0;i<IKM.length;i++)
                LIKM[k++]=IKM[i];
        }

        return HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,SALT,LIKM);
    }

    static byte[] LabeledExpand(byte[] PRK,String label,byte[] INFO,int L) {
        byte[] ar = HMAC.inttoBytes(L, 2);
        String rfc="RFCXX" +"XX ";
        byte[] prefix=(rfc+label).getBytes();
        byte[] LINFO = new byte[2+prefix.length+INFO.length];
        LINFO[0]=ar[0];
        LINFO[1]=ar[1];
        int k=2;
        for (int i=0;i<prefix.length;i++)
            LINFO[k++]=prefix[i];
        for (int i=0;i<INFO.length;i++)
            LINFO[k++]=INFO[i];

        return HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,PRK,LINFO);
    }

    static byte[] ExtractAndExpand(byte[] DH,byte[] CONTEXT) {
        byte[] PRK=LabeledExtract(null,"dh",DH);
        return LabeledExpand(PRK,"prk",CONTEXT,CONFIG_CURVE.HASH_TYPE);
    }

    public static byte[] encap(int config_id,byte[] SKE,byte[] pkE,byte[] pkR) {
        byte[] skE = new byte[GROUP];
        byte[] DH = new byte[FIELD];
        byte[] kemcontext = new byte[2*pkE.length];
	    int kem = config_id&255;

		for (int i=0;i<GROUP;i++) 
			skE[i]=SKE[i];
		if (kem==32 || kem==33) {
			reverse(skE);
			if (kem==32) {
	            skE[GROUP-1]&=248;
				skE[0]&=127;
				skE[0]|=64;
			} else {
				skE[GROUP-1]&=252;
				skE[0]|=128;
			}
		}
	 
	    ECDH.KEY_PAIR_GENERATE(null, skE, pkE);
	    if (kem==32 || kem==33) 
		    reverse(pkR);

	    ECDH.SVDP_DH(skE, pkR, DH);
	    if (kem==32 || kem==33) {
		    reverse(pkR);
		    reverse(pkE);
		    reverse(DH);
	    }
        int k=0;
        for (int i=0;i<pkE.length;i++)
            kemcontext[k++]=pkE[i];
        for (int i=0;i<pkR.length;i++)
            kemcontext[k++]=pkR[i];

    	return ExtractAndExpand(DH,kemcontext);
    }

    public static byte[] decap(int config_id,byte[] pkE,byte[] SKR) {
        byte[] skR = new byte[GROUP];
        byte[] DH = new byte[FIELD];
        byte[] kemcontext = new byte[2*pkE.length];
        byte[] pkR = new byte[pkE.length];
	    int kem = config_id&255;

	    for (int i=0;i<GROUP;i++)
		    skR[i]=SKR[i];
	    if (kem==32 || kem==33) {
		    reverse(skR);
		    reverse(pkE);
		    if (kem==32) {
			    skR[GROUP-1]&=248;
			    skR[0]&=127;
			    skR[0]|=64;
		    } else {
			    skR[GROUP-1]&=252;
			    skR[0]|=128;
		    }
	    }
	    ECDH.SVDP_DH(skR, pkE, DH);
	    if (kem==32 || kem==33) {
		    reverse(pkE);
		    reverse(DH);
	    }
        ECDH.KEY_PAIR_GENERATE(null,skR,pkR);
	    if (kem==32 || kem==33) {
		    reverse(pkR);
	    }
        int k=0;
        for (int i=0;i<pkE.length;i++)
            kemcontext[k++]=pkE[i];
        for (int i=0;i<pkR.length;i++)
            kemcontext[k++]=pkR[i];

    	return ExtractAndExpand(DH,kemcontext);
    }

    public static byte[] authEncap(int config_id,byte[] SKE,byte[] pkE,byte[] pkR,byte[] SKS) {
        int pklen=pkE.length;
        byte[] skE = new byte[GROUP];
        byte[] DH = new byte[FIELD];
        byte[] DH1 = new byte[FIELD];
        byte[] skS = new byte[GROUP];
        byte[] pkS = new byte[pklen];
        byte[] kemcontext = new byte[3*pklen];
	
	    int kem = config_id&255;

		for (int i=0;i<GROUP;i++) {
			skE[i]=SKE[i];
			skS[i]=SKS[i];
		}	
		if (kem==32 || kem==33) {
			reverse(skE);
			reverse(skS);
			if (kem==32) {
				skE[GROUP-1]&=248;
				skE[0]&=127;
				skE[0]|=64;
				skS[GROUP-1]&=248;
				skS[0]&=127;
				skS[0]|=64;	
			} else {
				skE[GROUP-1]&=252;
				skE[0]|=128;
				skS[GROUP-1]&=252;
				skS[0]|=128;
			}
		}
	    
	    ECDH.KEY_PAIR_GENERATE(null, skE, pkE);
	    if (kem==32 || kem==33) 
		    reverse(pkR);
	
	    ECDH.SVDP_DH(skE, pkR, DH);
	    ECDH.SVDP_DH(skS, pkR, DH1);

	    if (kem==32 || kem==33) {
		    reverse(DH);
		    reverse(DH1);
	    }

	    byte[] ZZ = new byte[2*FIELD];
	    for (int i=0;i<FIELD;i++) {
		    ZZ[i] = DH[i];
            ZZ[FIELD+i]= DH1[i];
	    }

        ECDH.KEY_PAIR_GENERATE(null,skS,pkS);

        if (kem==32 || kem==33)
        {
            reverse(pkR);
		    reverse(pkE);
            reverse(pkS);
        }
	    for (int i=0;i<pklen;i++) {
		    kemcontext[i] = pkE[i];
            kemcontext[pklen+i]= pkR[i];
            kemcontext[2*pklen+i]= pkS[i];
	    }

	    return ExtractAndExpand(ZZ,kemcontext);
    }

    public static byte[] authDecap(int config_id,byte[] pkE,byte[] SKR,byte[] pkS) {
        int pklen=pkE.length;
        byte[] skR = new byte[GROUP];
        byte[] DH = new byte[FIELD];
        byte[] DH1 = new byte[FIELD];
        byte[] pkR = new byte[pklen];
        byte[] kemcontext = new byte[3*pklen];

		int kem = config_id&255;

	    for (int i=0;i<GROUP;i++) 
		    skR[i]=SKR[i];
	
	    if (kem==32 || kem==33) {
		    reverse(skR);
		    reverse(pkE);
		    reverse(pkS);
		    if (kem==32) {
			    skR[GROUP-1]&=248;
			    skR[0]&=127;
			    skR[0]|=64;
		    } else {
		    	skR[GROUP-1]&=252;
			    skR[0]|=128;
		    }  
	    }
	    ECDH.SVDP_DH(skR, pkE, DH);
	    ECDH.SVDP_DH(skR, pkS, DH1);

	    if (kem==32 || kem==33) {
		    reverse(DH);
		    reverse(DH1);
	    }

	    byte[] ZZ = new byte[2*FIELD];
	    for (int i=0;i<FIELD;i++) {
		    ZZ[i] = DH[i];
            ZZ[FIELD+i]= DH1[i];
	    }
        ECDH.KEY_PAIR_GENERATE(null,skR,pkR);

        if (kem==32 || kem==33)
        {
            reverse(pkR);
		    reverse(pkE);
            reverse(pkS);
        }
	    for (int i=0;i<pklen;i++) {
		    kemcontext[i] = pkE[i];
            kemcontext[pklen+i]= pkR[i];
            kemcontext[2*pklen+i]= pkS[i];
	    }
	    return ExtractAndExpand(ZZ,kemcontext);
    }

    public static void keySchedule(int config_id,byte[] key,byte[] nonce,byte[] exp_secret,int mode,byte[] Z,byte[] info,byte[] psk,byte[] pskID) {
        int ctxlen=7+2*CONFIG_CURVE.HASH_TYPE;
	    byte[] context = new byte[ctxlen];
	    int kem_id=config_id&255;
	    int kdf_id=(config_id>>8)&3;
	    int aead_id=(config_id>>10)&3;
        int k=0;
	
	    byte[] ar=HMAC.inttoBytes(kem_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    ar=HMAC.inttoBytes(kdf_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    ar=HMAC.inttoBytes(aead_id, 2);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

	    ar = HMAC.inttoBytes(mode, 1);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

        byte[] ZEROS = new byte[CONFIG_CURVE.HASH_TYPE];
        for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++)
            ZEROS[i]=0;

        byte[] H=LabeledExtract(ZEROS,"pskID_hash",pskID);
        for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++)
            context[k++]=H[i];
        H=LabeledExtract(ZEROS,"info",info);
        for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++)
            context[k++]=H[i];

        if (psk==null)
            H=LabeledExtract(ZEROS,"psk_hash",ZEROS);
        else
            H=LabeledExtract(ZEROS,"psk_hash",psk);
        byte[] secret=LabeledExtract(H,"zz",Z);

        byte[] ex=LabeledExpand(secret,"key",context,CONFIG_CURVE.AESKEY);
        for (int i=0;i<ex.length;i++) key[i]=ex[i];

        ex=LabeledExpand(secret,"nonce",context,12);
        for (int i=0;i<ex.length;i++) nonce[i]=ex[i];

        if (exp_secret!=null)
        {
            ex=LabeledExpand(secret,"exp",context,CONFIG_CURVE.HASH_TYPE);
            for (int i=0;i<ex.length;i++) exp_secret[i]=ex[i];
        }
    }
}


