/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/* test driver and function exerciser for BLS API Functions */

package main

import "fmt"

import "github.com/miracl/amcl/version3/go/amcl"

import "github.com/miracl/amcl/version3/go/amcl/BN254"
import "github.com/miracl/amcl/version3/go/amcl/BLS383"
import "github.com/miracl/amcl/version3/go/amcl/BLS24"
import "github.com/miracl/amcl/version3/go/amcl/BLS48"

func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}

func bls_BN254(rng *amcl.RAND) {

	const BGS = BN254.BGS
	const BFS = BN254.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 4 * BFS /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	BN254.Init()

	BN254.KeyPairGenerate(rng, S[:], W[:])
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BN254.Sign(SIG[:], mess, S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res := BN254.Verify(SIG[:], mess, W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS383(rng *amcl.RAND) {

	const BGS = BLS383.BGS
	const BFS = BLS383.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 4 * BFS /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	BLS383.Init()

	BLS383.KeyPairGenerate(rng, S[:], W[:])
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS383.Sign(SIG[:], mess, S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res := BLS383.Verify(SIG[:], mess, W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS24(rng *amcl.RAND) {

	const BGS = BLS24.BGS
	const BFS = BLS24.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 8 * BFS /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	BLS24.Init()

	BLS24.KeyPairGenerate(rng, S[:], W[:])
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS24.Sign(SIG[:], mess, S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res := BLS24.Verify(SIG[:], mess, W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS48(rng *amcl.RAND) {

	const BGS = BLS48.BGS
	const BFS = BLS48.BFS
	const G1S = BFS + 1  /* Group 1 Size */
	const G2S = 16 * BFS /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	BLS48.Init()

	BLS48.KeyPairGenerate(rng, S[:], W[:])
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS48.Sign(SIG[:], mess, S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res := BLS48.Verify(SIG[:], mess, W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func main() {
	rng := amcl.NewRAND()
	var raw [100]byte
	for i := 0; i < 100; i++ {
		raw[i] = byte(i + 1)
	}
	rng.Seed(100, raw[:])

	bls_BN254(rng)
	bls_BLS383(rng)
	bls_BLS24(rng)
	bls_BLS48(rng)
}
