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
	config_id:=0x2A
	var pkE [C25519.EFS]byte

	INFO,_:=hex.DecodeString("4f6465206f6e2061204772656369616e2055726e")
	psk,_:=hex.DecodeString("6d656c6c6f6e")
	pskID,_:=hex.DecodeString("456e6e796e20447572696e206172616e204d6f726961")
	PLAIN,_:=hex.DecodeString("4265617574792069732074727574682c20747275746820626561757479")
	AAD,_:=hex.DecodeString("436f756e742d30")

// Mode 0
    mode:=0;
    fmt.Printf("\nMode 0\n");
	skR,_:=hex.DecodeString("139c8d38df6d8dfa2c8de98af621667c76c3f63f65c7c3966c4258c316f05033")
	skE,_:=hex.DecodeString("7227b3fee6b8e01b293b826ec8671b69894a1142981bb1513afa42819f2a22ef")
	pkR,_:=hex.DecodeString("186c394e175b7b161760b1bd5b822a0804bd066b170c695c0df123176fa7df6f")

	Z:=C25519.Encap(config_id,nil,skE,pkE[:],pkR)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.Decap(config_id,pkE[:],skR)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE:=C25519.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,nil,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG:=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 1
    mode=1;
    fmt.Printf("\nMode 1\n");
	skR,_=hex.DecodeString("fdf4507c4507ececd2b2232a761ce2cdb50d993b6216e6cb9538a1fc53baed02")
	skE,_=hex.DecodeString("0dc7bf88c1849fd7800f8bd52eed75f26160a3cb9d634190c48ced1daa4c341a")
	pkR,_=hex.DecodeString("696977f50e929c99287852ded3fc9b2709f8cd51c0d270c45c338dece959f437")

	Z=C25519.Encap(config_id,nil,skE,pkE[:],pkR)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.Decap(config_id,pkE[:],skR)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=C25519.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,psk,pskID,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 2

    mode=2;
    fmt.Printf("\nMode 2\n");
	skR,_=hex.DecodeString("a3f26ded71d69e6b7c4924e5756388efce4c8857b89cd00e492cd5a5778c40d7")
	skI,_:=hex.DecodeString("42962c361fe2e92343113407b95abe4eebef311e315726c579fc6fb7f22e16c5")
	skE,_=hex.DecodeString("372c79a960e1c33c6b6f69c7f4c63567f743a018d5d04fc4a243e0af2aeb3708")
	pkR,_=hex.DecodeString("39c07ff4a258dfe61a1e9e23ef1d9d39efce89c5326da949a9dfa3b11b9f2f2e")
	pkI,_:=hex.DecodeString("9888840b794623840275e594ec76a526e690b9111d21cdec3fe2faf18227f676")

	Z=C25519.AuthEncap(config_id,nil,skE,pkE[:],pkR,skI)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.AuthDecap(config_id,pkE[:],skR,pkI)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=C25519.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,nil,nil,pkI)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 3

    mode=3;
    fmt.Printf("\nMode 3\n");
	skR,_=hex.DecodeString("2d7c739195ba102216de162f9435991aa3ad42aeefdb7e22391ae34bae7e5a13")
	skI,_=hex.DecodeString("59c77f5734aef369f30d83c7e30c6bf372e120391cdaf13f34c915030284b75d")
	skE,_=hex.DecodeString("6827bbf4f7ebd0182a8ebc0ea364c7ddae1d1c8a4f58a903fa9f9f9d4228b126")
	pkR,_=hex.DecodeString("cc980df06e532bdb6b957f9f5a5caf55c55f46822cdfbd97e76f6ad4c62b322b")
	pkI,_=hex.DecodeString("db6ee4a53276b7bc90657cdde514f948af83c140540797ec717881490afed921")

	Z=C25519.AuthEncap(config_id,nil,skE,pkE[:],pkR,skI)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=C25519.AuthDecap(config_id,pkE[:],skR,pkI)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=C25519.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,psk,pskID,pkI)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

}

func hpke_NIST521(rng *core.RAND) {
	config_id:=0x53

	var pkE [2*NIST521.EFS+1]byte

	INFO,_:=hex.DecodeString("4f6465206f6e2061204772656369616e2055726e")
	psk,_:=hex.DecodeString("6d656c6c6f6e")
	pskID,_:=hex.DecodeString("456e6e796e20447572696e206172616e204d6f726961")
	PLAIN,_:=hex.DecodeString("4265617574792069732074727574682c20747275746820626561757479")
	AAD,_:=hex.DecodeString("436f756e742d30")

// Mode 0
    mode:=0;
    fmt.Printf("\nMode 0\n");
	skR,_:=hex.DecodeString("011d0d02be6fe4eb49654d5641f0ebb40b594aca84a7c1e76faa1fa0f5f3582502d29fd140d07e71f538c75b93c8ca416af95ad79c5e7e2c0889adce490367492aee")
	skE,_:=hex.DecodeString("0099f6326effd78a5c54eeba93e911f35bdd418f2e1de3281ba2172aebf251ff73614598c81686775c20618721c8c84466f2f6ca7b33636a3edbc69697087f8b414a")
	pkR,_:=hex.DecodeString("04015c0e75e50b266e5fb8e3a3f317608bbf64843c47cca29faa3b894a4f9b95ba533bd40d343e5747f9e85d9ffee5b4ed803a20a23ff0a246ca4d9d59eae9c38291d000c396eade14ed2d42fafe7c4be26da6f07af8b6902af5fbb39f8b0b15c7120d7f57f46ed24e711dda2469f418377d8cc8de7ec8b0594e155b8d6d6ca699f99a4189")

	Z:=NIST521.Encap(config_id,nil,skE,pkE[:],pkR)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.Decap(config_id,pkE[:],skR)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE:=NIST521.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,nil,nil,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG:=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 1
    mode=1;
    fmt.Printf("\nMode 1\n");
	skR,_=hex.DecodeString("00953f712b5c288be50460d0372612f88c3da171196a4cd958b0f658e988aa56ce5edc018e5899acda38649fc51049610bc8d1423189a9b5ff5f7e4fe1081df3c7b3")
	skE,_=hex.DecodeString("00416c4a6236475f01a0cd0a9a7a0d7229be88a63281fca350ea98d4438de57a23bd5053572bf0f7bed8bc84aba6f7322cabc6b2540e789eb5823660eaad8e206fd9")
	pkR,_=hex.DecodeString("040108f48ce8fcd71e7e07270fa566cfd66d9b7ab124df141d700b82141264a01306b94265afc3c361c0c0dc514864d4b12e687697d24ddce2f0c77f9c257b8a2f5a380109cb328c87f9833f729967a74675acc31bccdccf5016d442e7b33d2ecaf6f79d8a0e79d8259b07cd173e320ca25c68acf5aa4f3793e7fc6d076baa953060847a02")

	Z=NIST521.Encap(config_id,nil,skE,pkE[:],pkR)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.Decap(config_id,pkE[:],skR)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=NIST521.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,psk,pskID,nil)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 2
    mode=2;
    fmt.Printf("\nMode 2\n");
	skR,_=hex.DecodeString("006ae74c6d37982c4a6087500b66948a715ca971e7aa43260bae4683d78818cfc728b0d72d6834f4d401f35db13f932e414b44d03071805fcbd513a57130e18e8323")
	skI,_:=hex.DecodeString("01d9581f65c8cf1a90f1711fe377c15e68f534be11ea5e0158a8adebaa04f0be9c80d0f2517abf0cd117d9ca2073b604743076cee2405f4db2825ace05e0eae83354")
	skE,_=hex.DecodeString("0195e8805187cf89fc17007d90a75dff2dc9ae824aea70adf001ef539832932f0d8cd7d3bbc94e712fb64d0e5f0acbb7cb79e5bde9d24304c8b4ed0091c8905da986")
	pkR,_=hex.DecodeString("040054116ccfb36d9cd99e59b100ef9dbc70a6992b38632ee7650659275cdcab37bce7e74f2381cc7292ba418051432c3a8aeb706b3c05fcb886b3a95a306ae9863f5900e9b6db3150e7241fec23c607db539e1a2b2c1898b5d2b78ec1a3254bf022dee6e8c2f6265f71ac8e6003614accb8532dd58d5a07a959bde06b763f2f41a9c3ac32")
	pkI,_:=hex.DecodeString("0400fdb5f8a16b80e8a61c967f4f4f6b83740fc3521cd1654ee7e1cc42f4f645a57e68942303a48d37e09b7cf2be067f03ed0b7076fe0eda129bc65209c83bafbc0b5d012ba9db99b61236f645076e7f8b10c8763517dfcefd07241e90aa6a595209fc6aafc227fd9d870c8c6b8d037dd5386513608f7779887e47db733fe97f74169d21c7")

	Z=NIST521.AuthEncap(config_id,nil,skE,pkE[:],pkR,skI)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.AuthDecap(config_id,pkE[:],skR,pkI)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=NIST521.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,nil,nil,pkI)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

	CIPHER,TAG=core.GCM_ENCRYPT(KEY,NONCE,AAD,PLAIN)
	fmt.Printf("Cipher= "); printBinary(CIPHER[:])
	fmt.Printf("Tag= "); printBinary(TAG[:])

// Mode 3
    mode=3;
    fmt.Printf("\nMode 3\n");
	skR,_=hex.DecodeString("010bf0c9af1d5dda4c97151d6a9425c8f590aa7c0adff53c06d23380ae82bbd32f64ddf0c344d221d2f7657711d73de7fa25a75bd8fa75662029e118087276a5ef0b")
	skI,_=hex.DecodeString("01800af579dbd0c91a09fa0ff3ddf21d9a1447649528d777e00962c748159e6de03880a70331fdab67761e25f78977128f81c7fee9897eea74b5ee80e94414b6aed6")
	skE,_=hex.DecodeString("013e65c19df676b1d7da923475c72c359fbdd91f224d68785bdf5891bbadfd136a33cc8c31408b9652be389f52e8a19d9401aedaf549a0654f246c277af48f59b2ba")
	pkR,_=hex.DecodeString("0400a1735a659c6b281e603f170b5e886ccfff35d274b042d43a815447fc80208f599704d63a67355b4d04bfdc367f60961f1b36b1e11bf3ed7599f58948f364a1023501ec6b5acd7ce1cc2c3ec6dba8d72b08e472809623ac2df65dcb658dbd7d5f599c9ac624517d780b49b3d7619f5647be76a56a73fe2c3fc2ae620159cb1b7a437a94")
	pkI,_=hex.DecodeString("0400f0fe8e86b8d86d02a4dc5db6988f6343067174caff2e2700834b98d25045013eb24cf00c03dd506d562625d0fe5c576910d176705cd4ba1fcd10f5a98f466a0a2200a629f9f62f6053b554bf09b2a547b844f3e040c2b92c548babbc73cf05e77f23d0cffa9e5dfd0a57f9be64bf453ec48cbd00f2e47349572fead07dc73658256331")

	Z=NIST521.AuthEncap(config_id,nil,skE,pkE[:],pkR,skI)
	fmt.Printf("pkE= "); printBinary(pkE[:])
	fmt.Printf("Encapsulated Z= "); printBinary(Z[:])

	Z=NIST521.AuthDecap(config_id,pkE[:],skR,pkI)
	fmt.Printf("Decapsulated Z= "); printBinary(Z[:])

	KEY,NONCE=NIST521.KeySchedule(config_id,mode,pkR,Z,pkE[:],INFO,psk,pskID,pkI)
	fmt.Printf("Key= "); printBinary(KEY[:])
	fmt.Printf("Nonce= "); printBinary(NONCE[:])

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
