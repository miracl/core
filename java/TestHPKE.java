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

/* test driver and function exerciser for HPKE API Functions */

package org.miracl.core.XXX;  //

import junit.framework.TestCase;       //
import org.miracl.core.RAND;
import org.miracl.core.GCM;
import org.miracl.core.HMAC;

public class TestHPKE extends TestCase { 

    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    private static byte[] hexDecodeString(String s) {
        byte[] b = new byte[s.length() / 2];
        for (int i = 0; i < b.length; i++) {
            int j = i * 2;
            int v = Integer.parseInt(s.substring(j, j + 2), 16);
            b[i] = (byte) v;
        }
        return b;
    }

    public void testHPKE() {
        String packageName = this.getClass().getPackage().getName();

        if (packageName != "org.miracl.core.C25519" && packageName != "org.miracl.core.NIST521") {
            System.out.println("HPKE not specified for this curve");
            return;
        }
        int EGS = ECDH.EGS;
        int EFS = ECDH.EFS;
        int EAS = CONFIG_CURVE.AESKEY;
        int sha = CONFIG_CURVE.HASH_TYPE;

        byte[] seedE;
        byte[] seedR;
        byte[] seedS;

	    byte[] skR = new byte[EGS];
	    byte[] skE = new byte[EGS];
        byte[] skS = new byte[EGS];

        byte[] Z;

	    int config_id;
        int pksize;
        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) {
            config_id=0x520; // its C25519
            pksize=EFS;
        } else {
            config_id=0xB12; // its NIST521
            pksize=2*EFS+1;
        }
        byte[] pkE = new byte[pksize];
        byte[] pkR = new byte[pksize];
        byte[] pkS = new byte[pksize];

	    byte[] INFO=hexDecodeString("4f6465206f6e2061204772656369616e2055726e");
	    byte[] psk=hexDecodeString("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82");
	    byte[] pskID=hexDecodeString("456e6e796e20447572696e206172616e204d6f726961");
	    byte[] plain=hexDecodeString("4265617574792069732074727574682c20747275746820626561757479");
	    byte[] aad=hexDecodeString("436f756e742d30");
        System.out.println("\nTesting HPKE code");

        byte[] cipher= new byte[plain.length];
        byte[] tag= new byte[16];
        byte[] nonce= new byte[12];
        byte[] key=new byte[EAS];
        byte[] exp_secret=new byte[sha];

// Mode 0
        int mode=0;
        System.out.println("\nMode 0");

        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) {        
            seedE=hexDecodeString("1dacee520c81ade608f4fa3e5ccae0ecedcc7880e3fc6f3e5afd2e4af8396571");
            seedR=hexDecodeString("0a3367dadc97e200074936b5adedcd5680f30672d1ec7158fdfcb795040ec909");
        } else {
	        seedE=hexDecodeString("bb8f0c6c0d81a0a54d6084e3d7d8001875ce2b4831400075246ddeabc3f62d065d91fa8f2ca0cfee3a94f1fa6f43f53a2e39241ccd5ad9f0d889aba103e78286ca17");
	        seedR=hexDecodeString("28b1403adcf3d2bba7d7c733df4ef8ab9f4c2cf92c840beb7de3fd139611a95be850589a49825450c033b2e1f6cb66127c83827d5464fca1e427c2bb31bf18e857e2");
        }
        HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
        HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);

	    Z=HPKE.encap(config_id,skE,pkE,pkR);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.decap(config_id,skR,pkE,pkR);
	    System.out.print("Decapsulated Z= "); printBinary(Z); 

	    HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	    System.out.print("Key= "); printBinary(key);
	    System.out.print("Nonce= "); printBinary(nonce);
	    System.out.print("Exporter secret= "); printBinary(exp_secret);

	    GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	    System.out.print("Cipher= "); printBinary(cipher);
	    System.out.print("Tag= "); printBinary(tag);

// Mode 1
        mode=1;
        System.out.println("\nMode 1");

        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) {        
            seedE=hexDecodeString("7f5b697a87fb11d733e31265410c80b72525637e1b872f9ede00d507c0c534d7");
            seedR=hexDecodeString("89a6ce4b7b8eb12eebff3864615a2539e3e64fa2d4ab525c18c7bc451a2b5e40");
        } else {
	        seedE=hexDecodeString("bd12d21f5a36063ec0242aa26f22144aa7d24c3cc2e6a1805868250d8e9ce6fb2a04f8852cd6ec3241249fcf262b8f245d179636fcfbf275a75539cfc756a4dd3f53");
	        seedR=hexDecodeString("e9a016952866c33da399e64ecd629cf32826285698726f2c22debd4799b8fac112ac1a546ce39452406396a389fb10f51036c7022dc1cdde177b54d005731fb79ce4");
        }
        HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
        HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);

	    Z=HPKE.encap(config_id,skE,pkE,pkR);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.decap(config_id,skR,pkE,pkR);
	    System.out.print("Decapsulated Z= "); printBinary(Z); 

	    HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	    System.out.print("Key= "); printBinary(key);
	    System.out.print("Nonce= "); printBinary(nonce);
	    System.out.print("Exporter secret= "); printBinary(exp_secret);

	    GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	    System.out.print("Cipher= "); printBinary(cipher);
	    System.out.print("Tag= "); printBinary(tag);

// Mode 2
        mode=2;
        System.out.println("\nMode 2");

        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) {        
            seedE=hexDecodeString("f7faa81395a22af1071b5273f9f94db0609ab9a7f4eca54834ea408d97294cc2");
            seedR=hexDecodeString("a10d107753f85bf7128768693e53129fb44a281ea4849a86231632db48dbb28c");
            seedS=hexDecodeString("e1257ce84dd98bca7847d75661b658cd2cc6c4efbf9e989a40f390720b57d409");
        } else {
	        seedE=hexDecodeString("0e5ec115c172744e793ef333abfd5d9838fd31eef595f0b77a2f5623404aa42f0e77198c2ac78defeae75cc79cf5dc5c7587b6eda733e48f3b17d5e3496482f19cdb");
            seedR=hexDecodeString("3f3c12492eee6f1f8029a89002b6539ea9754b48412c14d2fe94368cf1fa473f0171cdb1117aaa447ad6914bfb794ead14a9cd22cd83caa22bd905bd7d614d401ddc");
	        seedS=hexDecodeString("02d1581b120d0415bda1fb2cbe6a3c81edf28209f0fc1460745ad9018747c31e6d42a36ed1734214312311db80bb3f88c2da6ea7624cf2915d32f2993ebeb92c2934");
        }
        HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
        HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
        HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	    Z=HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	    System.out.print("Decapsulated Z= "); printBinary(Z); 

	    HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,null,null);
	    System.out.print("Key= "); printBinary(key);
	    System.out.print("Nonce= "); printBinary(nonce);
	    System.out.print("Exporter secret= "); printBinary(exp_secret);

	    GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	    System.out.print("Cipher= "); printBinary(cipher);
	    System.out.print("Tag= "); printBinary(tag);

// Mode 3
        mode=3;
        System.out.println("\nMode 3");

        if (CONFIG_CURVE.CURVETYPE==CONFIG_CURVE.MONTGOMERY) {        
            seedE=hexDecodeString("e98713f7e24e7025fb7abddfff317d865ad5f4c3b6be22cda48dbcdde6151a68");
            seedR=hexDecodeString("b281654cdb2cd6a9670c3035eaa702736a951585a938381198f7d29085ca7f3a");
            seedS=hexDecodeString("ea95c4d3c5e7868af29fc0cb0841485dead8d54b57ee1b8ed31a153295454835");
        } else {
	        seedE=hexDecodeString("c393a0a0cab3313f0526d72b8bef55b26a156493263fa1a653069ed5f2f3637f371b15704778c89a6e01c3fd958dfee5f85613e2d4b2eb17a152810690d8252df44b");
            seedR=hexDecodeString("e7fa07c9938a3d9d883d256abd7fb5777ed7c4195a8d244c0e682fd1a08fdcce58ab1aa7ba31b1fa120f03d9df438509a1e31992506dbc196ac4489079b3b64bea9b");
	        seedS=hexDecodeString("403e8d5ff0155c71232a887f542df221c385956eaca3acceffdc4681b99c7e9613f90ba03b0c03c0c57a90b2dd31effc6278ab6dc059f3e5389dff6495bda12b4da4");
        }
        HPKE.DeriveKeyPair(config_id,skE,pkE,seedE);
        HPKE.DeriveKeyPair(config_id,skR,pkR,seedR);
        HPKE.DeriveKeyPair(config_id,skS,pkS,seedS);

	    Z=HPKE.authEncap(config_id,skE,skS,pkE,pkR,pkS);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.authDecap(config_id,skR,pkE,pkR,pkS);
	    System.out.print("Decapsulated Z= "); printBinary(Z); 

	    HPKE.keySchedule(config_id,key,nonce,exp_secret,mode,Z,INFO,psk,pskID);
	    System.out.print("Key= "); printBinary(key);
	    System.out.print("Nonce= "); printBinary(nonce);
	    System.out.print("Exporter secret= "); printBinary(exp_secret);

	    GCM.encrypt(cipher,tag,key,nonce,aad,plain);
	    System.out.print("Cipher= "); printBinary(cipher);
	    System.out.print("Tag= "); printBinary(tag);
    }
}
