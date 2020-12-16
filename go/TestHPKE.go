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

package main

import "fmt"
import "encoding/hex"

import "github.com/miracl/core/go/core"
import "github.com/miracl/core/go/core/C25519"
import "github.com/miracl/core/go/core/NIST521"

func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}

func hpke_C25519(rng *core.RAND) {
	config_id:=0x520
	var skE [C25519.EGS]byte
	var skR [C25519.EGS]byte
	var skS [C25519.EGS]byte
	var pkE [C25519.EFS]byte
	var pkR [C25519.EFS]byte
	var pkS [C25519.EFS]byte

	INFO,_:=hex.DecodeString("4f6465206f6e2061204772656369616e2055726e")
	psk,_:=hex.DecodeString("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82")
	pskID,_:=hex.DecodeString("456e6e796e20447572696e206172616e204d6f726961")
	PLAIN,_:=hex.DecodeString("4265617574792069732074727574682c20747275746820626561757479")
	AAD,_:=hex.DecodeString("436f756e742d30")

// Mode 0
    mode:=0;
    fmt.Printf("\nMode 0\n");
	seedE,_:=hex.DecodeString("6305de86b3cec022fae6f2f2d2951f0f90c8662112124fd62f17e0a99bdbd08e")
	seedR,_:=hex.DecodeString("6d9014e4609687b0a3670a22f2a14eac5ae6ad8c0beb62fb3ecb13dc8ebf5e06")
	C25519.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	C25519.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)

	Z:=C25519.Encap(config_id,skE[:],pkE[:],pkR[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.Decap(config_id,skR[:],pkE[:],pkR[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET:=C25519.KeySchedule(config_id,mode,Z,INFO,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG:=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 1
    mode=1;
    fmt.Printf("\nMode 1\n");
	seedE,_=hex.DecodeString("abd63dfd2fb9ccf8789cae5a6aff91e07f9f5925c27f005e702bf956b0000a85")
	seedR,_=hex.DecodeString("654e8b44e8e29fc75f3beadf7f28dc065e38a53c1a731e15f2d46fd6130574da")
	C25519.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	C25519.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)

	Z=C25519.Encap(config_id,skE[:],pkE[:],pkR[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.Decap(config_id,skR[:],pkE[:],pkR[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=C25519.KeySchedule(config_id,mode,Z,INFO,psk,pskID)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 2

    mode=2;
    fmt.Printf("\nMode 2\n");
	seedE,_=hex.DecodeString("42d691088397246b00e9d9ce8f5406a317433558dc28132e02618970005d02fd")
	seedR,_=hex.DecodeString("fb953f486ef5f7a1ceddff40bffe02b857c8af9d611966e417a24d6efa7c9d1c")
	seedS,_:=hex.DecodeString("131aa907c85b05726e7a058b064bf29cb2cb72a2afbffbd8076a884291f3143e")
	C25519.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	C25519.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)
	C25519.DeriveKeyPair(config_id,skS[:],pkS[:],seedS)

	Z=C25519.AuthEncap(config_id,skE[:],skS[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.AuthDecap(config_id,skR[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=C25519.KeySchedule(config_id,mode,Z,INFO,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 3

    mode=3;
    fmt.Printf("\nMode 3\n");
	seedE,_=hex.DecodeString("0c3a14fe896a7166f4d5e2a21c903b69f9ca71783290ca10f1b8c5eb258149be")
	seedR,_=hex.DecodeString("b759021868fba28c1ddd509eaac450a896ab0f5edffead0a019fecb574950d64")
	seedS,_=hex.DecodeString("2e7219703b6659698e4c2d141d13e0092df7039212db9c97e347c7d2c0aee239")
	C25519.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	C25519.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)
	C25519.DeriveKeyPair(config_id,skS[:],pkS[:],seedS)

	Z=C25519.AuthEncap(config_id,skE[:],skS[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.AuthDecap(config_id,skR[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=C25519.KeySchedule(config_id,mode,Z,INFO,psk,pskID)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

}

func hpke_NIST521(rng *core.RAND) {
	config_id:=0xB12
	var skE [NIST521.EGS]byte
	var skR [NIST521.EGS]byte
	var skS [NIST521.EGS]byte
	var pkE [2*NIST521.EFS+1]byte
	var pkR [2*NIST521.EFS+1]byte
	var pkS [2*NIST521.EFS+1]byte

	INFO,_:=hex.DecodeString("4f6465206f6e2061204772656369616e2055726e")
	psk,_:=hex.DecodeString("0247fd33b913760fa1fa51e1892d9f307fbe65eb171e8132c2af18555a738b82")
	pskID,_:=hex.DecodeString("456e6e796e20447572696e206172616e204d6f726961")
	PLAIN,_:=hex.DecodeString("4265617574792069732074727574682c20747275746820626561757479")
	AAD,_:=hex.DecodeString("436f756e742d30")

// Mode 0
    mode:=0;
    fmt.Printf("\nMode 0\n");
	seedE,_:=hex.DecodeString("ea39fec1941c5f516e19533f40d415c65fde023c10c559f3845e71ffccea478101573d069cc67874d5b2aba6a22eb51cdd689836b7e9cabbb4469c57947db7316fa7")
	seedR,_:=hex.DecodeString("8249fd42416aba5b0d51dcd3548d774ae172148cbba1519107c5d84a160225441a9c018fdf3b9ffc2c41c1c62e29208d5165a59e7f14fe93b4f911cbbebda1904391")
	NIST521.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	NIST521.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)

	Z:=NIST521.Encap(config_id,skE[:],pkE[:],pkR[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.Decap(config_id,skR[:],pkE[:],pkR[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET:=NIST521.KeySchedule(config_id,mode,Z,INFO,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG:=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 1
    mode=1;
    fmt.Printf("\nMode 1\n");
	seedE,_=hex.DecodeString("b563555965facaa37a5e754cf2e50193953e1d527e61637e521df1868354799258f0d15d8807750cea08e9b6a358d2440ab06725861b237fb69973cf41e802434433")
	seedR,_=hex.DecodeString("2be954cd856c0c548ec6de490c821be20c4fc9610b5dba1831c698045fa01dbceb49f8324bae26e5d1ee62db3246d65492812e2c539b96bc580d46c247304adfc55e")
	NIST521.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	NIST521.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)

	Z=NIST521.Encap(config_id,skE[:],pkE[:],pkR[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.Decap(config_id,skR[:],pkE[:],pkR[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=NIST521.KeySchedule(config_id,mode,Z,INFO,psk,pskID)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 2
    mode=2;
    fmt.Printf("\nMode 2\n");
	seedE,_=hex.DecodeString("c9621b9ef899275dc970606a2b0806fe860f62d539f3ee618a9409009b8ae154bc4acf495dd9fa8f850c4dca82b923b42270d7b16ed343c7e86e3036c88d0d7d77ee")
	seedR,_=hex.DecodeString("ef805c20cda1fa06e06cfb968ad68c748a3dd94337f7357ca0060a382a84fa5de6df3e3216f886957694547264d5bc63450cfdcc4d2b33fc8ebf8d7c708f8b5e4bb5")
	seedS,_:=hex.DecodeString("d8779e14425887ebb21b1952b1a0b77842830aef910724b082807dfebc8ec309b4969da762369e77834593970215b85510c9a0347ff14c8583aae7c9c2208275b740")
	NIST521.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	NIST521.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)
	NIST521.DeriveKeyPair(config_id,skS[:],pkS[:],seedS)

	Z=NIST521.AuthEncap(config_id,skE[:],skS[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.AuthDecap(config_id,skR[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=NIST521.KeySchedule(config_id,mode,Z,INFO,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 3
    mode=3;
    fmt.Printf("\nMode 3\n");
	seedE,_=hex.DecodeString("d7537fd470c0beece615e26dd109922460292e973127cb4e22da16c0756fc336224e07dbecdf36edd144ebcd82aece3db52f814a33a639b5e7c964b27f6e3195cd73")
	seedR,_=hex.DecodeString("f0858f5e1865db4fe45dc3274bcd273a29088d80f9203a16ec1210e3d81dd50a99f15c427d547fea55593e2ef834beb5f80c536fdd2881a8943c05488a371a3c988d")
	seedS,_=hex.DecodeString("1e8d0026273feb61537181872e03ed2c7756f0ed1c4bb9ecd159614c2afdcaacc5fcf70f6d30d7ea6760c98a1ce1138a82497eb72461ca5da50c8729d431de53857f")
	NIST521.DeriveKeyPair(config_id,skE[:],pkE[:],seedE)
	NIST521.DeriveKeyPair(config_id,skR[:],pkR[:],seedR)
	NIST521.DeriveKeyPair(config_id,skS[:],pkS[:],seedS)

	Z=NIST521.AuthEncap(config_id,skE[:],skS[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.AuthDecap(config_id,skR[:],pkE[:],pkR[:],pkS[:])
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE,EXP_SECRET=NIST521.KeySchedule(config_id,mode,Z,INFO,psk,pskID)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])
	fmt.Printf("Exporter Secret= "); printBinary(EXP_SECRET[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

}

func main() {
	rng := core.NewRAND()
	var raw [100]byte
	for i := 0; i < 100; i++ {
		raw[i] = byte(i)
	}
	rng.Seed(100, raw[:])

	fmt.Printf("\nTesting HPKE for curve C25519\n")
	hpke_C25519(rng)
	fmt.Printf("\nTesting HPKE for curve NIST521\n")
	hpke_NIST521(rng)
}
