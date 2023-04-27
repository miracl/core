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

/* test driver and function exerciser for EDDSA API Functions */

package main

import "fmt"
import "encoding/hex"
import "miracl/core"
import "miracl/core/ED25519"
import "miracl/core/ED448"

func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}

func prehashSHA2(M []byte) []byte {
	sh:=core.NewHASH512()
    for i:=0;i<len(M);i++ {
        sh.Process(M[i])
	}
    return sh.Hash()
}

func prehashSHA3(M []byte) [64]byte {
	var digest [64]byte
	sh:=core.NewSHA3(core.SHA3_SHAKE256)
	for i:=0;i<len(M);i++ {
		sh.Process(M[i])
	}
	sh.Shake(digest[:],64)
	return digest
}

func eddsa_ED25519(rng *core.RAND) {
	var Q [ED25519.EFS]byte
	var SIG [2*ED25519.EFS]byte
	prehash := false;

	fmt.Printf("\nTesting EDDSA for curve ED25519\n")

	D, _ := hex.DecodeString("c5aa8df43f9f837bedb7442f31dcb7b166d38535076f094b85ce3a2e0b4458f7")
	M := [2]byte{0xAF,0x82}

	/* Generate Key pair D/Q */
	ED25519.KEY_PAIR_GENERATE(nil, D, Q[:])

	fmt.Printf("Private key= 0x")
	printBinary(D)
	fmt.Printf("Public key= 0x")
	printBinary(Q[:])

    if prehash {
        HM := prehashSHA2(M[:]);
        ED25519.SIGNATURE(true,D, nil,HM[:], SIG[:])
    } else {
        ED25519.SIGNATURE(false,D, nil,M[:], SIG[:])
    }
    fmt.Printf("Signature= 0x"); printBinary(SIG[:])
 
    var verify bool
    if prehash {
        HM := prehashSHA2(M[:])
        verify=ED25519.VERIFY(true,Q[:], nil ,HM[:], SIG[:])
    } else {
        verify=ED25519.VERIFY(false,Q[:], nil ,M[:], SIG[:])
    }
    if verify {
        fmt.Printf("Signature is verified\n")
    } else {
        fmt.Printf("Signature is NOT verified\n")
	}
}

func eddsa_ED448(rng *core.RAND) {
	var Q [ED448.EFS+1]byte
	var SIG [2*ED448.EFS+2]byte
	prehash := false;

	fmt.Printf("\nTesting EDDSA for curve ED448\n")

	D, _ := hex.DecodeString("c4eab05d357007c632f3dbb48489924d552b08fe0c353a0d4a1f00acda2c463afbea67c5e8d2877c5e3bc397a659949ef8021e954e0a12274e")
	M := [1]byte{0x03}
	//CTX:=[3]byte{0x66,0x6F,0x6F};
	CTX:=[0]byte{}

	/* Generate Key pair D/Q */
	ED448.KEY_PAIR_GENERATE(nil, D, Q[:])

	fmt.Printf("Private key= 0x")
	printBinary(D)
	fmt.Printf("Public key= 0x")
	printBinary(Q[:])

    if prehash {
        HM := prehashSHA3(M[:]);
        ED448.SIGNATURE(true,D, CTX[:],HM[:], SIG[:])
    } else {
        ED448.SIGNATURE(false,D, CTX[:],M[:], SIG[:])
    }
    fmt.Printf("Signature= 0x"); printBinary(SIG[:])

    var verify bool
    if prehash {
        HM := prehashSHA3(M[:])
        verify=ED448.VERIFY(true,Q[:], CTX[:] ,HM[:], SIG[:])
    } else {
        verify=ED448.VERIFY(false,Q[:], CTX[:] ,M[:], SIG[:])
    }
    if verify {
        fmt.Printf("Signature is verified\n")
    } else {
        fmt.Printf("Signature is NOT verified\n")
	}
}

func main() {
	rng := core.NewRAND()
	var raw [100]byte
	for i := 0; i < 100; i++ {
		raw[i] = byte(i)
	}
	rng.Seed(100, raw[:])
	eddsa_ED25519(rng)
	eddsa_ED448(rng)
}
