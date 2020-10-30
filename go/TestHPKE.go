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
	seedE,_:=hex.DecodeString("591c66abd531b9c8287cf76ac053efba38d61e994e7f656c30dab6723a8af9ce")
	seedR,_:=hex.DecodeString("8a219e9a42233826f165d2c1036399fa84cfb3bcb93872bc49287dfbe6f1fec9")
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
	seedE,_=hex.DecodeString("f39542a6e45dc0e6225de671455fea9d918d4fa241c7fb301895a138ce7c2f52")
	seedR,_=hex.DecodeString("5aa0335739cb3dbafce48d5d9cccd7d499b0727b445522e1408aba8171aabfae")
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
	seedE,_=hex.DecodeString("0b119a7a132355f3c2a0ad03e10e252d7292fcde480e726d03a81f127d02c050")
	seedR,_=hex.DecodeString("874ba77ded181cd1cb4f2b4f37386a5b2b86c84c57d2a6d1b93a05fb5edd69f0")
	seedS,_:=hex.DecodeString("2e369ba803dcee17dba926e4252b8dd9a47417a35bcf90f0e55e4af85c83153b")
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
	seedE,_=hex.DecodeString("96360b18f7822f85f4a0a1dd200d00ab82ad0032b05a1f34cb120a0cbda4865a")
	seedR,_=hex.DecodeString("48710faa5c8b8b2a4447c9ccf1cc232fb49f6c0fd04b151949757e15b414eea8")
	seedS,_=hex.DecodeString("dd413f8d2d4b195a7cd0793453d04660203a51db65a57d16c2a720e6d44e526c")
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
	seedE,_:=hex.DecodeString("3fdb7eab3a6a99de8abbcb507be5704ae6a8994008b8a8e6b63fbd97fa8619c66bd8665c22079939f3f63f978c5806802b22ba5bb396da9cf252ee67068bf57461bf")
	seedR,_:=hex.DecodeString("2e99ac709379c7eb15ca068253bbae4dd6297c2397f47a89b8cb3ef4e83f235f83cb1ce3d2f754c47431ff0d0d8d2c429a7b6768d9524c3be60b9fb7749c49cb816b")
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
	seedE,_=hex.DecodeString("ae300665d34d5ab7c0508a94a741ba2cb285966106ba9cefbe1f9c24c3eb626108d0c9ccc8291d90c50c6d04ac181ccd8efc2cc52383eb205637a84d2be5362bf247")
	seedR,_=hex.DecodeString("dbbda5e5a54ee85cfe076e4746ceb971bed338a7fe0b625d6c7c28b3c82c8128258906a52543655ecb62889e9dbe8feec747d06e4216f88ca4adba200179e0e75276")
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
	seedE,_=hex.DecodeString("11c0c7337b294452826e14a7f6c9e7981a03c467a08f47a8b478b37f3e9c90266898e3c3f8e84235a6a2837269c84b355d7f5ca133085172a08f00c3857da8a1410b")
	seedR,_=hex.DecodeString("bb56b3452a80ad82b2d48c19ce76194a198eefdb67040959bc9e479db0682a4b5b46d7d020df66864d374b25deb5927144e3f08f2f9eacdd5f54b8b5c65d91ee211f")
	seedS,_:=hex.DecodeString("bab663b9c05f680f401a494ae8c8714fd95cbcd56a01e9e8194b4b3da863a5e8313d4916dc58f6d3aaa2dafe420ae81b2a6c0075223afc6b13f3734a26ca30da5e38")
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
	seedE,_=hex.DecodeString("4ebb461a5c46330d6de3a40d19ac499cc206733cf1a4fb3ba922d976aa1c45848668f04a3b5a4845a0d1c83755967d8914a9824fbb8823d161c16c93c51636e1ad89")
	seedR,_=hex.DecodeString("1ae2f1008c46c7a6e9275b1e29c906475c6bc019b1dfc38cbce68c5233de9d33ba93fe9d7b9ea5beb04f4adc5a3b72238f6e3d904d29eb0680ea240103d3335a3c47")
	seedS,_=hex.DecodeString("e0f2ada4f2a900fded767dc9868119ee3e4767afac667a780b68e5e2b4d7d363dbf02717ab314369c45f34dcec3de384a65e8453a971ad0353a507f34dc1d5d9b8f5")
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
