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
            seedE=hexDecodeString("6305de86b3cec022fae6f2f2d2951f0f90c8662112124fd62f17e0a99bdbd08e");
            seedR=hexDecodeString("6d9014e4609687b0a3670a22f2a14eac5ae6ad8c0beb62fb3ecb13dc8ebf5e06");
        } else {
	        seedE=hexDecodeString("ea39fec1941c5f516e19533f40d415c65fde023c10c559f3845e71ffccea478101573d069cc67874d5b2aba6a22eb51cdd689836b7e9cabbb4469c57947db7316fa7");
	        seedR=hexDecodeString("8249fd42416aba5b0d51dcd3548d774ae172148cbba1519107c5d84a160225441a9c018fdf3b9ffc2c41c1c62e29208d5165a59e7f14fe93b4f911cbbebda1904391");
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
            seedE=hexDecodeString("abd63dfd2fb9ccf8789cae5a6aff91e07f9f5925c27f005e702bf956b0000a85");
            seedR=hexDecodeString("654e8b44e8e29fc75f3beadf7f28dc065e38a53c1a731e15f2d46fd6130574da");
        } else {
	        seedE=hexDecodeString("b563555965facaa37a5e754cf2e50193953e1d527e61637e521df1868354799258f0d15d8807750cea08e9b6a358d2440ab06725861b237fb69973cf41e802434433");
	        seedR=hexDecodeString("2be954cd856c0c548ec6de490c821be20c4fc9610b5dba1831c698045fa01dbceb49f8324bae26e5d1ee62db3246d65492812e2c539b96bc580d46c247304adfc55e");
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
            seedE=hexDecodeString("42d691088397246b00e9d9ce8f5406a317433558dc28132e02618970005d02fd");
            seedR=hexDecodeString("fb953f486ef5f7a1ceddff40bffe02b857c8af9d611966e417a24d6efa7c9d1c");
            seedS=hexDecodeString("131aa907c85b05726e7a058b064bf29cb2cb72a2afbffbd8076a884291f3143e");
        } else {
	        seedE=hexDecodeString("c9621b9ef899275dc970606a2b0806fe860f62d539f3ee618a9409009b8ae154bc4acf495dd9fa8f850c4dca82b923b42270d7b16ed343c7e86e3036c88d0d7d77ee");
            seedR=hexDecodeString("ef805c20cda1fa06e06cfb968ad68c748a3dd94337f7357ca0060a382a84fa5de6df3e3216f886957694547264d5bc63450cfdcc4d2b33fc8ebf8d7c708f8b5e4bb5");
	        seedS=hexDecodeString("d8779e14425887ebb21b1952b1a0b77842830aef910724b082807dfebc8ec309b4969da762369e77834593970215b85510c9a0347ff14c8583aae7c9c2208275b740");
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
            seedE=hexDecodeString("0c3a14fe896a7166f4d5e2a21c903b69f9ca71783290ca10f1b8c5eb258149be");
            seedR=hexDecodeString("b759021868fba28c1ddd509eaac450a896ab0f5edffead0a019fecb574950d64");
            seedS=hexDecodeString("2e7219703b6659698e4c2d141d13e0092df7039212db9c97e347c7d2c0aee239");
        } else {
	        seedE=hexDecodeString("d7537fd470c0beece615e26dd109922460292e973127cb4e22da16c0756fc336224e07dbecdf36edd144ebcd82aece3db52f814a33a639b5e7c964b27f6e3195cd73");
            seedR=hexDecodeString("f0858f5e1865db4fe45dc3274bcd273a29088d80f9203a16ec1210e3d81dd50a99f15c427d547fea55593e2ef834beb5f80c536fdd2881a8943c05488a371a3c988d");
	        seedS=hexDecodeString("1e8d0026273feb61537181872e03ed2c7756f0ed1c4bb9ecd159614c2afdcaacc5fcf70f6d30d7ea6760c98a1ce1138a82497eb72461ca5da50c8729d431de53857f");
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
