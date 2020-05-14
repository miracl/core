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

	    byte[] skR;
	    byte[] skE;
	    byte[] pkR;
        byte[] skS;
        byte[] pkS;
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
            skR=hexDecodeString("d3c8ca6516cd4cc75f66210c5a49d05381bfbfc0de090c19432d778ea4599829");
            skE=hexDecodeString("b9d453d3ec0dbe59fa4a193bde3e4ea17f80c9b2fa69f2f3e029120303b86885");
            pkR=hexDecodeString("10b2fc2332b75206d2c791c3db1094dfd298b6508138ce98fec2c0c7a4dbc408");
        } else {
	        skR=hexDecodeString("01b3e33a2cf926ef496fff7e86a3743c23a9797c3bf99af8366ce22b3b0940fff1f1934e4d0bd539e130bdc3ff5991d37bdf45fc73529fe9a2e00e138d376610c26a");
	        skE=hexDecodeString("00252116681dacf4e707e9881aff5496942a36e74185347df21ab9d647dfe2a910e325b027ac3114335c038589d86a6a1a665498a5a6ea687ad5ddbd4258f273e2e7");
	        pkR=hexDecodeString("0400ec315d49eee4579d51967cb9cf2a848d918059769f530f0fcfb92342bbf00b561a55dc58fae5f8ef0fcf53e86514600b09787dc886afbbd682feb5cd3d1b3e671800be3c0358bf880ba2435eada8a1d5ea3585ea920c29ae39a5cb035de057721b8fc07d0666d46a6be634b257427404a19c7ebface8fa37da857bbef6fe04622d9627");
        }

	    Z=HPKE.encap(config_id,skE,pkE,pkR);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.decap(config_id,pkE,skR);
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
            skR=hexDecodeString("1f68f688b24c27f825012d40efa4fb33e033d717d569047a702c3ef5a64bde64");
            skE=hexDecodeString("8ad4455d6eda442a9731ac224c9f8a468f489c77e3871cde9ebdd12e9027bad7");
            pkR=hexDecodeString("6f61735fee57c59ce6489d80a11d77b7b2f9e9fddc3cb0bff0cf5a982ce7f344");
        } else {
	        skR=hexDecodeString("017ee8d2bb7dc4766d91596bc1fa741d03ac78c729258e9118c65a2b21d9fcb4249b3854049a384c8c3c18387ae4d94b57e11407359d62cb5de0e7c2eeded7c5ee32");
	        skE=hexDecodeString("009c521217c269a1c94169f6bcd3b399903ea6d385b38d51eac1fa07a4aac06bc144613ba531e3377a817715d987fd63229e3adcaa1e585e3fe75ea8288a486ae3a8");
	        pkR=hexDecodeString("04009070038197aaa0221dc91ad4c638ce2e28ba901d9cf588f4dcec4ae243507f19bf14c835d442aa26a9ee6d14f69777ae92c6f3a80f29bdd24ec38972a23ce0e89a00f81d57db61fb8f530c25fb9dd226bf57de3c914254baf5c57c2de46cded50e051531b0d5e7ea57c26a4ab80bba58e1b862110ad67b505e8f1011e00ad3f23a0d43");
        }

	    Z=HPKE.encap(config_id,skE,pkE,pkR);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.decap(config_id,pkE,skR);
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
            skR=hexDecodeString("41ee53399ebae982cd067fefa138b6b401cea1a7ef428fe9d6bc90e903a88f6e");
            skS=hexDecodeString("55799581f14ec785dbaa11857700adf78f842a51b5ae6a4b4e5a4d99c66e5793");
            skE=hexDecodeString("367299c0da446bfa8f3b41382c58b1b77fdadb5cf056d1fe94d6ab0b8741a184");
            pkR=hexDecodeString("777b10479021ffb3d21ff7ad0d7ff1a27220f6203e729826a71dc1dd7f77ed27");
            pkS=hexDecodeString("2bba172f178cf852e8670e574fdcdd62d8dcfa063548d3424d84f3e403f4e64c");
        } else {
	        skR=hexDecodeString("00e4a39e9808aec7cf16708aeb13eaa70fd0bcb453534233836f3207d88c61af1c23a4a993687973b42d178c1789bee1656f01d463e86cad6758c79f817f680e26bf");
            skS=hexDecodeString("01dd6f1ea81fbcb28da541d801edfa89e5648cc809b42cbe8b503caf6d7c5026a8bd9ea058951b8b9eee10e095a1865ef26ef247a3e62c25062f3877ff0627711eca");
	        skE=hexDecodeString("0179c8ecb90db0cec33df6cf79fda902d4facfd55a1e26216e176769d149df8da12d46f35f07c7cabd5613dc47826fc5b778f24a50381c4ed0a9b93cee969f32f735");
	        pkR=hexDecodeString("0400bd881c923366876ec86f5110ec695d2da0cd8b35e64adbd5a9b13fc6daa9daac5b38277b6cd791fd6b47deab09815492426f373ead722b2ea7257b9e110681ec1001800806a1c5f656bb354333c001d20a7639e5749fb3bc1a209f00e512752bceffa60e771926ce9d6972bb3abeaf6d314b09d3c0ae69452dd61db2b84a21d70402dc");
            pkS=hexDecodeString("040162d338cc04cf8f7aa40bf50efdef65fd17d561a22e387e833fa71408a9a3a7e44900964544c58ce2e3e444a4622fafdaa0ce72d39266b6aae61d01879783bd5d910063ec49234436b959148f8220e583774298e5dbd28be96be0c4f754ac93a10306c72495a84472d08376547d5fec4da477d316c4f6810cd97a9e97e792a06745fbc8");
        }

	    Z=HPKE.authEncap(config_id,skE,pkE,pkR,skS);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.authDecap(config_id,pkE,skR,pkS);
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
            skR=hexDecodeString("2fe8ef0b6fdf5f8da2b43472ca05bc324f7e6aabd9e2b65b2dfa5b892f832a20");
            skS=hexDecodeString("aede8f90d017a796c2dc73f3674168837a0ca0afaf3577b6aeeb784dc0b31c49");
            skE=hexDecodeString("84a883acd803f41ea16ec23e81ebe3af3cff34fa3c6b50616d67511404d3daa0");
            pkR=hexDecodeString("09171abdb6c6c8833791cb29357e39c2dbd5d6a7c1ed726f77bbdca5eddf397a");
            pkS=hexDecodeString("92bf662e7ae5fda99130c32334f556803b00a419bb726386017c1fe217fb0e3d");
        } else {
	        skR=hexDecodeString("00dcca4c7aad3a047a5d0c579dce56d45a6ef7446055ef19d4295dff1b8d1586f95e09052f733c5d1c9fa8bb7eab289cc3c5bb23e55559606a7b509d56bd660a63d0");
            skS=hexDecodeString("002a28ddb89ecbb9af391e31185cdaf7a4683c9bf3f950866b73e65cae7f769a849a792b23c65266cda967208a6f7d741ce06b9a5b206a1c670a36a02c0b2d62bbc5");
	        skE=hexDecodeString("00a69a328f366433eb173d1cd308627e6733d7b52d88745eb8bd63a4d4959d6ff82389a1ead33fa9816afcb348532d78d41e1444f9f1aa945cdfc4f4bb98c8514722");
	        pkR=hexDecodeString("0400475fd0db79dacda40f43684ccc8a3397689135f422388905a95f7fc4357fad0ef3dec49c98a5aaa722457e4ee51d4eca72845b2d0580a8e77ab5086c3a9c6edede00c5bfa1b71a8c8ba3672952e5ceebc0446df94c96cc0051faa671158d5c9c86a05797c3ca6ccc19173b09bf2fe9b8e052f4ffecc871df03dc1da15b00cb1bfaa003");
            pkS=hexDecodeString("040031d4c9313147b1b3e76ae4721a1fd12b8ff0096f5b4301e062e52d59d5cbebf1826b424e4ce29ebf726fd67425451506f3a38fe9b5d38c967ef4f03d9b4ae55e4901449a96bc684065b5e10e7852b43b7fbba72e3ecd3daf740c1b32289194beed37aa233c70ef8ae5cccd2ff65cbce812135e7dc5771b37506f1671f98dede8641dd8");
        }

	    Z=HPKE.authEncap(config_id,skE,pkE,pkR,skS);
	    System.out.print("pkE= "); printBinary(pkE);
	    System.out.print("Encapsulated Z= "); printBinary(Z);    

	    Z=HPKE.authDecap(config_id,pkE,skR,pkS);
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
