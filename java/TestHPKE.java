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
	    byte[] psk=hexDecodeString("5db3b80a81cb63ca59470c83414ef70a");
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
            seedE=hexDecodeString("a77ae3e14cc2ec9e603a9049423d48e66a5e3139e896e95cf19919430657adc7");
            seedR=hexDecodeString("1289f0db1d8f68d0c531b5e53a40911a2a2347059355d7c267717033fef2b08c");
        } else {
	        seedE=hexDecodeString("1ed3768f499b5b3c2beda3166528b649d4b117a0bd450f0e9e19815c2597d1777ac67ea367415fb28c8819c94b383a0a8a15c9f03b4835330e3e6c8bc8319202e473");
	        seedR=hexDecodeString("62641514bccd2858f3d6513305288d6ca0e443f00a86eb33ccd519d1803aebc5d07bbad0e1013ce61d9c9d713b3c90c8e79a1af01d6c69750f67cbbd1d9d4afeedfa");
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
            seedE=hexDecodeString("0fa1407ccee05de0cceb2f2d2381d2df0602dbd43be90eefd288ce4ad0b3ba32");
            seedR=hexDecodeString("326ee379f778718e6cb343f55668fbb9d0098ba0503cd4414a8f1ce252605c39");
        } else {
	        seedE=hexDecodeString("64463def238f309f1e9d1f28c15dc126cffa4ded911a4c527eeb71ba593847fb405756239d2c694ce4effa3996cafb5cc0b3736dd988deb7289210ec92bf6b339302");
	        seedR=hexDecodeString("41b782c18c14986c9d7a636152f13677aeddf479c1c7791ea46e0ebbe35ca9dd524c23d730eef443741d7d965415833d6c549c8c1b31ad05f2b9a88f916b2930528e");
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
            seedE=hexDecodeString("02900cb4856b5f222293a9bd7bda2f1f81c562dc3002336ad1c39f6572402b7d");
            seedR=hexDecodeString("518df90f0f5044ce653180c700e4902d37a7ba1cd23482a76e18b300fecaac4e");
            seedS=hexDecodeString("262a05ad0c08030cdbbaafc03d64f33b95bf8089f216c62ac39b72064a4b4dcb");
        } else {
	        seedE=hexDecodeString("81dc51e31ef8e9b33fefcdf00bd3b0ff585b941fe76cf39a86c269e2f53be7edb3db0be1a58b6cb8d8e6020fe8a2018c59d47cacb35b2b8c61bd4155438b5eda5c0d");
            seedR=hexDecodeString("54af23ea93c8fc34deb6a7cd70e657ea8990fc4e9a18656d5764b62f7a33a9e0212adeae1585ad2ef28688c1b558866c1975973c4dff08955c1f9fd7939b10b5fbfc");
	        seedS=hexDecodeString("b65599d814192278ab826ef197a61b77db50f40495f77502dfaa03acd1f3565a3cefebd59de2328ece0638c90d8a89f9ca58f2850e39e9a4c9c339290d66da12fdf0");
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
            seedE=hexDecodeString("c1d1028243a951dbf6469025f3a1304407b08fb932104e61c7aab42ab4f1995c");
            seedR=hexDecodeString("02a965d8f53bbdcc11cc618d4f31f69277500b75959ca97fd533058315511d1b");
            seedS=hexDecodeString("e9c09a3e50073935e75d3846007a26088a93ebf58ad0bb30ad6c42a9d4d2419e");
        } else {
	        seedE=hexDecodeString("dc1fda9b21a1af6925ecf9ad79d2422f698b4168587c7908b36f5f58352181b9506554d8d8c9427e0dd2cfda25f0eabf58e9f5597e1b76ac12c799fe96e3cc03dc59");
            seedR=hexDecodeString("46592c2b171b8cdcce89601fab103f63ed43badadcf9df62a928ae3b7fa91f269eff3485f6401c374e19a8bb988005626b9c26d39795282b1095bcc4f62a67255e15");
	        seedS=hexDecodeString("d02446c344c10cd162486caa69aa1156ac3066e0fd668fa7faaf13bdbc944edbc0cd68ee36e4c30ecc36c2c5ab0978473eb1b5dcfff27985c9328877e85fd48b657d");
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
