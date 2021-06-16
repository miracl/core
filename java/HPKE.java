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

    static byte[] LabeledExtract(byte[] SALT,byte[] SUITE_ID,String label,byte[] IKM) {
        String rfc="HPKE-v1";

        byte[] RFC=rfc.getBytes();
        byte[] LABEL=label.getBytes();
        int IKMlen;
        if (IKM==null)
            IKMlen=0;
        else
            IKMlen=IKM.length;
        byte[] LIKM = new byte[RFC.length+SUITE_ID.length+LABEL.length+IKMlen];
        int k=0;
        for (int i=0;i<RFC.length;i++)
            LIKM[k++]=RFC[i];
        for (int i=0;i<SUITE_ID.length;i++)
            LIKM[k++]=SUITE_ID[i];
        for (int i=0;i<LABEL.length;i++)
            LIKM[k++]=LABEL[i];
        if (IKM!=null) {
            for (int i=0;i<IKM.length;i++)
                LIKM[k++]=IKM[i];
        }

        return HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,SALT,LIKM);
    }

    static byte[] LabeledExpand(byte[] PRK,byte[] SUITE_ID,String label,byte[] INFO,int L) {
        byte[] AR = HMAC.inttoBytes(L, 2);
        String rfc="HPKE-v1";
        byte[] RFC=rfc.getBytes();
        byte[] LABEL=label.getBytes();
        int INFOlen;
        if (INFO==null)
            INFOlen=0;
        else
            INFOlen=INFO.length;
        byte[] LINFO=new byte[2+RFC.length+SUITE_ID.length+LABEL.length+INFOlen];
        LINFO[0]=AR[0];
        LINFO[1]=AR[1];
        int k=2;
        for (int i=0;i<RFC.length;i++)
            LINFO[k++]=RFC[i];
        for (int i=0;i<SUITE_ID.length;i++)
            LINFO[k++]=SUITE_ID[i];
        for (int i=0;i<LABEL.length;i++)
            LINFO[k++]=LABEL[i];
        if (INFO!=null) {
            for (int i=0;i<INFO.length;i++)
                LINFO[k++]=INFO[i];
        }

        return HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,PRK,LINFO);
    }

    static byte[] ExtractAndExpand(int config_id,byte[] DH,byte[] CONTEXT) {
	    int kem = config_id&255;
        String txt="KEM";
        byte[] KEM_ID = HMAC.inttoBytes(kem, 2);
        byte[] KEM =txt.getBytes();
        byte[] SUITE_ID = new byte[2+KEM.length];
        int k=KEM.length;
        for (int i=0;i<k;i++)
            SUITE_ID[i]=KEM[i];
        SUITE_ID[k]=KEM_ID[0];
        SUITE_ID[k+1]=KEM_ID[1];

        byte[] PRK=LabeledExtract(null,SUITE_ID,"eae_prk",DH);
        return LabeledExpand(PRK,SUITE_ID,"shared_secret",CONTEXT,CONFIG_CURVE.HASH_TYPE);
    }

    public static boolean DeriveKeyPair(int config_id,byte[] SK,byte[] PK,byte[] SEED) {
        int counter=0;
        byte[] INFO=new byte[1];
	    int kem = config_id&255;
        String txt="KEM";
        byte[] KEM_ID = HMAC.inttoBytes(kem, 2);
        byte[] KEM =txt.getBytes();
        byte[] SUITE_ID = new byte[2+KEM.length];
        int k=KEM.length;
        for (int i=0;i<k;i++)
            SUITE_ID[i]=KEM[i];
        SUITE_ID[k]=KEM_ID[0];
        SUITE_ID[k+1]=KEM_ID[1];
        byte[] PRK=LabeledExtract(null,SUITE_ID,"dkp_prk",SEED);
        byte[] S;
		if (kem==32 || kem==33) { // RFC7748
           S=LabeledExpand(PRK,SUITE_ID,"sk",null,GROUP);
           reverse(S);
           if (kem==32)
           {
	            S[GROUP-1]&=248;
				S[0]&=127;
				S[0]|=64;
           } else {
				S[GROUP-1]&=252;
				S[0]|=128;
           }
        } else {
            int bit_mask;
            if (kem==18) bit_mask=1;
            else bit_mask=0xff;
            S=new byte[GROUP];
            while (!ECDH.IN_RANGE(S) && counter<256)
            {
                INFO[0]=(byte)counter;
                S=LabeledExpand(PRK,SUITE_ID,"candidate",INFO,GROUP);
                S[0]&=bit_mask;
                counter++;
            }
        }
        for (int i=0;i<GROUP;i++)
            SK[i]=S[i];
	    ECDH.KEY_PAIR_GENERATE(null, SK, PK);  
        if (kem==32 || kem==33)
            reverse(PK);
        if (counter<256) return true;
        return false;
    }

    public static byte[] encap(int config_id,byte[] skE,byte[] pkE,byte[] pkR) {
        byte[] DH = new byte[ECDH.EFS/*pkE.length*/];
        byte[] kemcontext = new byte[2*pkE.length];
	    int kem = config_id&255;

		if (kem==32 || kem==33) {
			reverse(pkR);
	        ECDH.SVDP_DH(skE, pkR, DH, 0);
            reverse(pkR);
            reverse(DH);
		} else {
	        ECDH.SVDP_DH(skE, pkR, DH, 0);
        }
	 
        int k=0;
        for (int i=0;i<pkE.length;i++)
            kemcontext[k++]=pkE[i];
        for (int i=0;i<pkR.length;i++)
            kemcontext[k++]=pkR[i];

    	return ExtractAndExpand(config_id,DH,kemcontext);
    }

    public static byte[] decap(int config_id,byte[] skR,byte[] pkE,byte[] pkR) {
        byte[] DH = new byte[ECDH.EFS/*pkE.length*/];
        byte[] kemcontext = new byte[2*pkE.length];
	    int kem = config_id&255;

	    if (kem==32 || kem==33) {
		    reverse(pkE);
	        ECDH.SVDP_DH(skR, pkE, DH, 0);
            reverse(pkE);
            reverse(DH);
	    } else {
            ECDH.SVDP_DH(skR, pkE, DH, 0);
        }

        int k=0;
        for (int i=0;i<pkE.length;i++)
            kemcontext[k++]=pkE[i];
        for (int i=0;i<pkR.length;i++)
            kemcontext[k++]=pkR[i];

    	return ExtractAndExpand(config_id,DH,kemcontext);
    }

    public static byte[] authEncap(int config_id,byte[] skE,byte[] skS,byte[] pkE,byte[] pkR,byte[] pkS) {
        int pklen=pkE.length;
        byte[] DH = new byte[ECDH.EFS];
        byte[] DH1 = new byte[ECDH.EFS];
        byte[] kemcontext = new byte[3*pklen];
	
	    int kem = config_id&255;

		if (kem==32 || kem==33) {
			reverse(pkR);
	        ECDH.SVDP_DH(skE, pkR, DH, 0);
	        ECDH.SVDP_DH(skS, pkR, DH1, 0);
			reverse(pkR);
            reverse(DH);
            reverse(DH1);
		} else {
	        ECDH.SVDP_DH(skE, pkR, DH, 0);
	        ECDH.SVDP_DH(skS, pkR, DH1, 0);
        }

	    byte[] ZZ = new byte[2*ECDH.EFS];
	    for (int i=0;i<ECDH.EFS;i++) {
		    ZZ[i] = DH[i];
            ZZ[ECDH.EFS+i]= DH1[i];
	    }

	    for (int i=0;i<pklen;i++) {
		    kemcontext[i] = pkE[i];
            kemcontext[pklen+i]= pkR[i];
            kemcontext[2*pklen+i]= pkS[i];
	    }

	    return ExtractAndExpand(config_id,ZZ,kemcontext);
    }

    public static byte[] authDecap(int config_id,byte[] skR,byte[] pkE,byte[] pkR,byte[] pkS) {
        int pklen=pkE.length;
        byte[] DH = new byte[ECDH.EFS];
        byte[] DH1 = new byte[ECDH.EFS];
        byte[] kemcontext = new byte[3*pklen];

		int kem = config_id&255;

	    if (kem==32 || kem==33) {
		    reverse(pkE);
		    reverse(pkS);
    	    ECDH.SVDP_DH(skR, pkE, DH, 0);
	        ECDH.SVDP_DH(skR, pkS, DH1, 0);
            reverse(pkE);
            reverse(pkS);
            reverse(DH);
            reverse(DH1);
	    } else {
	        ECDH.SVDP_DH(skR, pkE, DH, 0);
	        ECDH.SVDP_DH(skR, pkS, DH1, 0);
        }

	    byte[] ZZ = new byte[2*ECDH.EFS];
	    for (int i=0;i<ECDH.EFS;i++) {
		    ZZ[i] = DH[i];
            ZZ[ECDH.EFS+i]= DH1[i];
	    }

	    for (int i=0;i<pklen;i++) {
		    kemcontext[i] = pkE[i];
            kemcontext[pklen+i]= pkR[i];
            kemcontext[2*pklen+i]= pkS[i];
	    }
	    return ExtractAndExpand(config_id,ZZ,kemcontext);
    }

    public static void keySchedule(int config_id,byte[] key,byte[] nonce,byte[] exp_secret,int mode,byte[] Z,byte[] info,byte[] psk,byte[] pskID) {
        int ctxlen=1+2*CONFIG_CURVE.HASH_TYPE;
	    byte[] context = new byte[ctxlen];
        byte[] num;	

	    int kem = config_id&255;
	    int kdf=(config_id>>8)&3;
	    int aead=(config_id>>10)&3;
        String txt="HPKE"; 
        byte[] KEM =txt.getBytes();
        byte[] SUITE_ID = new byte[6+KEM.length];
        int k=KEM.length;
        for (int i=0;i<k;i++)
            SUITE_ID[i]=KEM[i];
        num = HMAC.inttoBytes(kem, 2);
        SUITE_ID[k++]=num[0];
        SUITE_ID[k++]=num[1];
        num = HMAC.inttoBytes(kdf, 2);
        SUITE_ID[k++]=num[0];
        SUITE_ID[k++]=num[1];
        num = HMAC.inttoBytes(aead, 2);
        SUITE_ID[k++]=num[0];
        SUITE_ID[k++]=num[1];
        k=0;
	    byte[] ar = HMAC.inttoBytes(mode, 1);
        for (int i=0;i<ar.length;i++)
		    context[k++] = ar[i];

        byte[] H=LabeledExtract(null,SUITE_ID,"psk_id_hash",pskID);
        for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++)
            context[k++]=H[i];
        H=LabeledExtract(null,SUITE_ID,"info_hash",info);
        for (int i=0;i<CONFIG_CURVE.HASH_TYPE;i++)
            context[k++]=H[i];
//System.out.print("context= 0x"); printBinary(context);
//        H=LabeledExtract(null,SUITE_ID,"psk_hash",psk);
//        byte[] secret=LabeledExtract(H,SUITE_ID,"secret",Z);

        byte[] secret=LabeledExtract(Z,SUITE_ID,"secret",psk);

//System.out.print("secret= 0x"); printBinary(secret);
        byte[] ex=LabeledExpand(secret,SUITE_ID,"key",context,CONFIG_CURVE.AESKEY);
        for (int i=0;i<ex.length;i++) key[i]=ex[i];

        ex=LabeledExpand(secret,SUITE_ID,"base_nonce",context,12);
        for (int i=0;i<ex.length;i++) nonce[i]=ex[i];

        if (exp_secret!=null)
        {
            ex=LabeledExpand(secret,SUITE_ID,"exp",context,CONFIG_CURVE.HASH_TYPE);
            for (int i=0;i<ex.length;i++) exp_secret[i]=ex[i];
        }
    }
}


