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

/* test driver and function exerciser for MPIN API Functions */

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


func mpin_BN254(rng *core.RAND) {

	const MGS = BN254.MGS
	const MFS = BN254.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 4*MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var HCID [G1S]byte
	var HSID [G1S]byte

	dst:="BN254G1_XMD:SHA-256_SVDW_NU_MPIN"
	DST:= []byte(dst)

// Trusted Authority set-up */
	fmt.Printf("\nTesting MPIN 2-factor authentication protocol on curve BN254\n")
	BN254.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

// Create Client Identity 
		IDstr := "testUser@miracl.com"
		CLIENT_ID := []byte(IDstr)
		BN254.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HCID[:])
		fmt.Printf("Client ID Hashed to Curve= ")
		printBinary(HCID[:])

// Client approaches Trusted Authority and is issued secret
	BN254.MPIN_GET_CLIENT_SECRET(S[:], HCID[:], TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])
// TA sends Client secret to Client

// Server is issued secret by TA
	BN254.MPIN_GET_SERVER_SECRET(S[:], SST[:])
//	fmt.Printf("Server Secret SS: 0x")
//	printBinary(SST[:])

// Client extracts PIN from secret to create Token 
		pin := 1234
		fmt.Printf("Client extracts PIN= %d", pin)
		fmt.Printf("\n")
		rtn := BN254.MPIN_EXTRACT_PIN(HCID[:], pin, TOKEN[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
			fmt.Printf("\n")
			return
		}
		fmt.Printf("Client Token TK: 0x")
		printBinary(TOKEN[:])

// Exercise Secret Sharing
		R:=make([]byte,128)
		for i:=0;i<128;i++ {
			R[i]=rng.GetByte()
		}
	// create 4 unique shares of TOKEN
		Sh1:=core.NewSHARE(1,3,TOKEN[:],R)  // indicate 3 shares required for recovery
		Sh2:=core.NewSHARE(2,3,TOKEN[:],R)
		// Sh3:=core.NewSHARE(3,3,TOKEN[:],R)	// must comment out or Go throws an error (Grrr..)
		Sh4:=core.NewSHARE(4,3,TOKEN[:],R)

		var Shares [3]*core.SHARE

		Shares[0]=Sh1  // any 3 shares to recover TOKEN
		Shares[1]=Sh2
		Shares[2]=Sh4
       
		for i:=0;i<len(TOKEN);i++ {
			TOKEN[i]=0
		}

		copy(TOKEN[:],core.Recover(Shares[:]))  // recover token

// MPin Protocol

// Client enters ID and PIN
		pin = 0
		fmt.Printf("PIN= ")
		fmt.Scanf("%d ", &pin)

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
		rtn = BN254.MPIN_CLIENT_1(HCID[:], rng, X[:], pin, TOKEN[:], SEC[:], xID[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
			return
		}

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
				BN254.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HSID[:])

// Server generates Random number Y and sends it to Client */
				BN254.MPIN_RANDOM_GENERATE(rng, Y[:])


		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
			rtn = BN254.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
			if rtn != 0 {
				fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
				return
			}

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

			rtn = BN254.MPIN_SERVER(HSID[:], Y[:], SST[:], xID[:], SEC[:])

			if rtn != 0 {
				if rtn == BN254.BAD_PIN {
					fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
				} else {
					fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
				}
			} else {
				fmt.Printf("Server says - PIN is good! You really are " + IDstr + "\n")
			}
	return
}

func mpin_BLS12383(rng *core.RAND) {
	const MGS = BLS12383.MGS
	const MFS = BLS12383.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 4*MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var HCID [G1S]byte
	var HSID [G1S]byte

	dst:="BLS12383G1_XMD:SHA-256_SVDW_NU_MPIN"
	DST:= []byte(dst)

// Trusted Authority set-up */
	fmt.Printf("\nTesting MPIN 2-factor authentication protocol on curve BLS12383\n")
	BLS12383.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

// Create Client Identity 
		IDstr := "testUser@miracl.com"
		CLIENT_ID := []byte(IDstr)
		BLS12383.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HCID[:])
		fmt.Printf("Client ID Hashed to Curve= ")
		printBinary(HCID[:])

// Client approaches Trusted Authority and is issued secret
	BLS12383.MPIN_GET_CLIENT_SECRET(S[:], HCID[:], TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])
// TA sends Client secret to Client

// Server is issued secret by TA
	BLS12383.MPIN_GET_SERVER_SECRET(S[:], SST[:])
//	fmt.Printf("Server Secret SS: 0x")
//	printBinary(SST[:])

// Client extracts PIN from secret to create Token 
		pin := 1234
		fmt.Printf("Client extracts PIN= %d", pin)
		fmt.Printf("\n")
		rtn := BLS12383.MPIN_EXTRACT_PIN(HCID[:], pin, TOKEN[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
			fmt.Printf("\n")
			return
		}
		fmt.Printf("Client Token TK: 0x")
		printBinary(TOKEN[:])

// MPin Protocol

// Client enters ID and PIN
		pin = 0
		fmt.Printf("PIN= ")
		fmt.Scanf("%d ", &pin)

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
		rtn = BLS12383.MPIN_CLIENT_1(HCID[:], rng, X[:], pin, TOKEN[:], SEC[:], xID[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
			return
		}

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
				BLS12383.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HSID[:])

// Server generates Random number Y and sends it to Client */
				BLS12383.MPIN_RANDOM_GENERATE(rng, Y[:])


		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
			rtn = BLS12383.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
			if rtn != 0 {
				fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
				return
			}

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

			rtn = BLS12383.MPIN_SERVER(HSID[:], Y[:], SST[:], xID[:], SEC[:])

			if rtn != 0 {
				if rtn == BLS12383.BAD_PIN {
					fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
				} else {
					fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
				}
			} else {
				fmt.Printf("Server says - PIN is good! You really are " + IDstr + "\n")
			}
	return
}

func mpin_BLS24479(rng *core.RAND) {
	const MGS = BLS24479.MGS
	const MFS = BLS24479.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 8*MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var HCID [G1S]byte
	var HSID [G1S]byte

	dst:="BLS24479G1_XMD:SHA-256_SVDW_NU_MPIN"
	DST:= []byte(dst)

// Trusted Authority set-up */
	fmt.Printf("\nTesting MPIN 2-factor authentication protocol on curve BLS24479\n")
	BLS24479.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

// Create Client Identity 
		IDstr := "testUser@miracl.com"
		CLIENT_ID := []byte(IDstr)
		BLS24479.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HCID[:])
		fmt.Printf("Client ID Hashed to Curve= ")
		printBinary(HCID[:])

// Client approaches Trusted Authority and is issued secret
	BLS24479.MPIN_GET_CLIENT_SECRET(S[:], HCID[:], TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])
// TA sends Client secret to Client

// Server is issued secret by TA
	BLS24479.MPIN_GET_SERVER_SECRET(S[:], SST[:])
//	fmt.Printf("Server Secret SS: 0x")
//	printBinary(SST[:])

// Client extracts PIN from secret to create Token 
		pin := 1234
		fmt.Printf("Client extracts PIN= %d", pin)
		fmt.Printf("\n")
		rtn := BLS24479.MPIN_EXTRACT_PIN(HCID[:], pin, TOKEN[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
			fmt.Printf("\n")
			return
		}
		fmt.Printf("Client Token TK: 0x")
		printBinary(TOKEN[:])

// MPin Protocol

// Client enters ID and PIN
		pin = 0
		fmt.Printf("PIN= ")
		fmt.Scanf("%d ", &pin)

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
		rtn = BLS24479.MPIN_CLIENT_1(HCID[:], rng, X[:], pin, TOKEN[:], SEC[:], xID[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
			return
		}

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
				BLS24479.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HSID[:])

// Server generates Random number Y and sends it to Client */
				BLS24479.MPIN_RANDOM_GENERATE(rng, Y[:])


		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
			rtn = BLS24479.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
			if rtn != 0 {
				fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
				return
			}

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

			rtn = BLS24479.MPIN_SERVER(HSID[:], Y[:], SST[:], xID[:], SEC[:])

			if rtn != 0 {
				if rtn == BLS24479.BAD_PIN {
					fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
				} else {
					fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
				}
			} else {
				fmt.Printf("Server says - PIN is good! You really are " + IDstr + "\n")
			}
	return
}

func mpin_BLS48556(rng *core.RAND) {
	const MGS = BLS48556.MGS
	const MFS = BLS48556.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 16*MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var HCID [G1S]byte
	var HSID [G1S]byte

	dst:="BLS48556G1_XMD:SHA-256_SVDW_NU_MPIN"
	DST:= []byte(dst)

// Trusted Authority set-up */
	fmt.Printf("\nTesting MPIN 2-factor authentication protocol on curve BLS48556\n")
	BLS48556.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

// Create Client Identity 
		IDstr := "testUser@miracl.com"
		CLIENT_ID := []byte(IDstr)
		BLS48556.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HCID[:])
		fmt.Printf("Client ID Hashed to Curve= ")
		printBinary(HCID[:])

// Client approaches Trusted Authority and is issued secret
	BLS48556.MPIN_GET_CLIENT_SECRET(S[:], HCID[:], TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])
// TA sends Client secret to Client

// Server is issued secret by TA
	BLS48556.MPIN_GET_SERVER_SECRET(S[:], SST[:])
//	fmt.Printf("Server Secret SS: 0x")
//	printBinary(SST[:])

// Client extracts PIN from secret to create Token 
		pin := 1234
		fmt.Printf("Client extracts PIN= %d", pin)
		fmt.Printf("\n")
		rtn := BLS48556.MPIN_EXTRACT_PIN(HCID[:], pin, TOKEN[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
			fmt.Printf("\n")
			return
		}
		fmt.Printf("Client Token TK: 0x")
		printBinary(TOKEN[:])

// MPin Protocol

// Client enters ID and PIN
		pin = 0
		fmt.Printf("PIN= ")
		fmt.Scanf("%d ", &pin)

// Client First pass: Inputs H(CLIENT_ID), RNG, pin, and TOKEN. Output x and U = x.H(CLIENT_ID) and re-combined secret SEC
		rtn = BLS48556.MPIN_CLIENT_1(HCID[:], rng, X[:], pin, TOKEN[:], SEC[:], xID[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
			return
		}

// Send CLIENT_ID and U=x.ID to server. Server hashes ID to curve.
				BLS48556.MPIN_ENCODE_TO_CURVE(DST,CLIENT_ID,HSID[:])

// Server generates Random number Y and sends it to Client */
				BLS48556.MPIN_RANDOM_GENERATE(rng, Y[:])


		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
			rtn = BLS48556.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
			if rtn != 0 {
				fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
				return
			}

// Server Second pass. Inputs H(CLIENT_ID), Y, -(x+y)*SEC, U and Server secret SST. 

			rtn = BLS48556.MPIN_SERVER(HSID[:], Y[:], SST[:], xID[:], SEC[:])

			if rtn != 0 {
				if rtn == BLS48556.BAD_PIN {
					fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
				} else {
					fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
				}
			} else {
				fmt.Printf("Server says - PIN is good! You really are " + IDstr + "\n")
			}
	return
}

func main() {
	rng := core.NewRAND()
	var raw [100]byte
	for i := 0; i < 100; i++ {
		raw[i] = byte(i)
	}
	rng.Seed(100, raw[:])

	mpin_BN254(rng)
	mpin_BLS12383(rng)
	mpin_BLS24479(rng)
	mpin_BLS48556(rng)
}
