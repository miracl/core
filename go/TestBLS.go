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

/* Test driver and function exerciser for Boneh-Lynn-Shacham BLS Signature API Functions */

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

See CPP library version for example

*/


package main

import "fmt"

import "github.com/miracl/core/go/core"
import "github.com/miracl/core/go/core/BN254"
import "github.com/miracl/core/go/core/BLS12383"
import "github.com/miracl/core/go/core/BLS24479"
import "github.com/miracl/core/go/core/BLS48556"

func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}

func bls_BN254(rng *core.RAND) {

	const BGS = BN254.BGS
	const BFS = BN254.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 2 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [32]byte

    for i:=0;i<len(IKM);i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature on BN254 curve\n")
	mess := "This is a test message"

	res := BN254.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BN254.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BN254.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BN254.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS12383(rng *core.RAND) {

	const BGS = BLS12383.BGS
	const BFS = BLS12383.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 2 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [32]byte

    for i:=0;i<len(IKM);i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature on BLS12383 curve\n")
	mess := "This is a test message"

	res := BLS12383.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS12383.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS12383.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS12383.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS24479(rng *core.RAND) {

	const BGS = BLS24479.BGS
	const BFS = BLS24479.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 4 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [48]byte

    for i:=0;i<len(IKM);i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature on BLS24479 curve\n")
	mess := "This is a test message"

	res := BLS24479.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS24479.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS24479.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS24479.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS48556(rng *core.RAND) {

	const BGS = BLS48556.BGS
	const BFS = BLS48556.BFS
	const G1S = BFS + 1  /* Group 1 Size */
	const G2S = 8 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [64]byte

    for i:=0;i<len(IKM);i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature on BLS48556 curve\n")
	mess := "This is a test message"

	res := BLS48556.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS48556.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS48556.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS48556.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func main() {
	rng := core.NewRAND()
	var raw [100]byte
	for i := 0; i < 100; i++ {
		raw[i] = byte(i + 1)
	}
	rng.Seed(100, raw[:])

	bls_BN254(rng)
	bls_BLS12383(rng)
	bls_BLS24479(rng)
	bls_BLS48556(rng)
}
