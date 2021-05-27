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
	seedE,_:=hex.DecodeString("7268600d403fce431561aef583ee1613527cff655c1343f29812e66706df3234")
	seedR,_:=hex.DecodeString("6db9df30aa07dd42ee5e8181afdb977e538f5e1fec8a06223f33f7013e525037")
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
	seedE,_=hex.DecodeString("78628c354e46f3e169bd231be7b2ff1c77aa302460a26dbfa15515684c00130b")
	seedR,_=hex.DecodeString("d4a09d09f575fef425905d2ab396c1449141463f698f8efdb7accfaff8995098")
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
	seedE,_=hex.DecodeString("6e6d8f200ea2fb20c30b003a8b4f433d2f4ed4c2658d5bc8ce2fef718059c9f7")
	seedR,_=hex.DecodeString("f1d4a30a4cef8d6d4e3b016e6fd3799ea057db4f345472ed302a67ce1c20cdec")
	seedS,_:=hex.DecodeString("94b020ce91d73fca4649006c7e7329a67b40c55e9e93cc907d282bbbff386f58")
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
	seedE,_=hex.DecodeString("4303619085a20ebcf18edd22782952b8a7161e1dbae6e46e143a52a96127cf84")
	seedR,_=hex.DecodeString("4b16221f3b269a88e207270b5e1de28cb01f847841b344b8314d6a622fe5ee90")
	seedS,_=hex.DecodeString("62f77dcf5df0dd7eac54eac9f654f426d4161ec850cc65c54f8b65d2e0b4e345")
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
	seedE,_:=hex.DecodeString("7f06ab8215105fc46aceeb2e3dc5028b44364f960426eb0d8e4026c2f8b5d7e7a986688f1591abf5ab753c357a5d6f0440414b4ed4ede71317772ac98d9239f70904")
	seedR,_:=hex.DecodeString("2ad954bbe39b7122529f7dde780bff626cd97f850d0784a432784e69d86eccaade43b6c10a8ffdb94bf943c6da479db137914ec835a7e715e36e45e29b587bab3bf1")
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
	seedE,_=hex.DecodeString("f3ebfa9a69a924e672114fcd9e06fa9559e937f7eccce4181a2b506df53dbe514be12f094bb28e01de19dd345b4f7ede5ad7eaa6b9c3019592ec68eaae9a14732ce0")
	seedR,_=hex.DecodeString("a2a2458705e278e574f835effecd18232f8a4c459e7550a09d44348ae5d3b1ea9d95c51995e657ad6f7cae659f5e186126a471c017f8f5e41da9eba74d4e0473e179")
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
	seedE,_=hex.DecodeString("fe1c589c2a05893895a537f38c7cb4300b5a7e8fef3d6ccb8f07a498029c61e90262e009dc254c7f6235f9c6b2fd6aeff0a714db131b09258c16e217b7bd2aa619b0")
	seedR,_=hex.DecodeString("8feea0438481fc0ecd470d6adfcda334a759c6b8650452c5a5dd9b2dd2cc9be33d2bb7ee64605fc07ab4664a58bb9a8de80defe510b6c97d2daf85b92cd4bb0a66bf")
	seedS,_:=hex.DecodeString("2f66a68b85ef04822b054ef521838c00c64f8b6226935593b69e13a1a2461a4f1a74c10c836e87eed150c0db85d4e4f506cbb746149befac6f5c07dc48a615ef92db")
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
	seedE,_=hex.DecodeString("54272797b1fbc128a6967ff1fd606e0c67868f7762ce1421439cbc9e90ce1b28d566e6c2acbce712e48eebf236696eb680849d6873e9959395b2931975d61d38bd6c")
	seedR,_=hex.DecodeString("3db434a8bc25b27eb0c590dc64997ab1378a99f52b2cb5a5a5b2fa540888f6c0f09794c654f4468524e040e6b4eca2c9dcf229f908b9d318f960cc9e9baa92c5eee6")
	seedS,_=hex.DecodeString("65d523d9b37e1273eb25ad0527d3a7bd33f67208dd1666d9904c6bc04969ae5831a8b849e7ff642581f2c3e56be84609600d3c6bbdaded3f6989c37d2892b1e978d5")
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
