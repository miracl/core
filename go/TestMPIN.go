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

/* Configure mode of operation */

const PERMITS bool = true
const PINERROR bool = true
const FULL bool = true
const SINGLE_PASS bool = false

func mpin_BN254(rng *core.RAND) {

	var sha = BN254.HASH_TYPE

	const MGS = BN254.MGS
	const MFS = BN254.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 4 * MFS +1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var PERMIT [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var xCID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var E [12 * MFS]byte
	var F [12 * MFS]byte
	var HID [G1S]byte
	var HTID [G1S]byte

	var G1 [12 * MFS]byte
	var G2 [12 * MFS]byte
	var R [MGS]byte
	var Z [G1S]byte
	var W [MGS]byte
	var T [G1S]byte
	var CK [BN254.AESKEY]byte
	var SK [BN254.AESKEY]byte

	var HSID []byte

	/* Trusted Authority set-up */

	fmt.Printf("\nTesting MPIN curve BN254\n")
	BN254.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

	/* Create Client Identity */
	IDstr := "testUser@miracl.com"
	CLIENT_ID := []byte(IDstr)

	HCID := BN254.MPIN_HASH_ID(sha, CLIENT_ID) /* Either Client or TA calculates Hash(ID) - you decide! */

	fmt.Printf("Client ID= ")
	printBinary(CLIENT_ID)
	//fmt.Printf("\n")

	/* Client and Server are issued secrets by DTA */
	BN254.MPIN_GET_SERVER_SECRET(S[:], SST[:])
	fmt.Printf("Server Secret SS: 0x")
	printBinary(SST[:])

	BN254.MPIN_GET_CLIENT_SECRET(S[:], HCID, TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])

	/* Client extracts PIN from secret to create Token */
	pin := 1234
	fmt.Printf("Client extracts PIN= %d", pin)
	fmt.Printf("\n")
	rtn := BN254.MPIN_EXTRACT_PIN(sha, CLIENT_ID, pin, TOKEN[:])
	if rtn != 0 {
		fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
		fmt.Printf("\n")
	}

	fmt.Printf("Client Token TK: 0x")
	printBinary(TOKEN[:])

	if FULL {
		BN254.MPIN_PRECOMPUTE(TOKEN[:], HCID, G1[:], G2[:])
	}

	date := 0
	if PERMITS {
		date = 42 //BN254.Today()
		/* Client gets "Time Token" permit from DTA */
		BN254.MPIN_GET_CLIENT_PERMIT(sha, date, S[:], HCID, PERMIT[:])
		fmt.Printf("Time Permit TP: 0x")
		printBinary(PERMIT[:])

		/* This encoding makes Time permit look random - Elligator squared */
		BN254.MPIN_ENCODING(rng, PERMIT[:])
		fmt.Printf("Encoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
		BN254.MPIN_DECODING(PERMIT[:])
		fmt.Printf("Decoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
	}
	pin = 0
	fmt.Printf("\nPIN= ")
	fmt.Scanf("%d ", &pin)

	pxID := xID[:]
	pxCID := xCID[:]
	pHID := HID[:]
	pHTID := HTID[:]
	pE := E[:]
	pF := F[:]
	pPERMIT := PERMIT[:]
	var prHID []byte

	if date != 0 {
		prHID = pHTID
		if !PINERROR {
			pxID = nil
			// pHID=nil
		}
	} else {
		prHID = pHID
		pPERMIT = nil
		pxCID = nil
		pHTID = nil
	}
	if !PINERROR {
		pE = nil
		pF = nil
	}

	if SINGLE_PASS {
		fmt.Printf("MPIN Single Pass\n")
		timeValue := BN254.MPIN_GET_TIME()
		rtn = BN254.MPIN_CLIENT(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT, timeValue, Y[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BN254.MPIN_HASH_ID(sha, CLIENT_ID)
			BN254.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		rtn = BN254.MPIN_SERVER(sha, date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF, CLIENT_ID, timeValue)
		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
		}

		if FULL {
			HSID = BN254.MPIN_HASH_ID(sha, CLIENT_ID)
			BN254.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}
	} else {
		fmt.Printf("MPIN Multi Pass\n")
		/* Send U=x.ID to server, and recreate secret from token and pin */
		rtn = BN254.MPIN_CLIENT_1(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT)
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BN254.MPIN_HASH_ID(sha, CLIENT_ID)
			BN254.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		/* Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp. */
		BN254.MPIN_SERVER_1(sha, date, CLIENT_ID, pHID, pHTID)

		/* Server generates Random number Y and sends it to Client */
		BN254.MPIN_RANDOM_GENERATE(rng, Y[:])

		if FULL {
			HSID = BN254.MPIN_HASH_ID(sha, CLIENT_ID)
			BN254.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}

		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
		rtn = BN254.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
		}

		/* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
		/* If PIN error not required, set E and F = null */

		rtn = BN254.MPIN_SERVER_2(date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF)

		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER_1 rtn: %d\n", rtn)
		}

		if rtn == BN254.BAD_PIN {
			fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
			if PINERROR {
				err := BN254.MPIN_KANGAROO(E[:], F[:])
				if err != 0 {
					fmt.Printf("(Client PIN is out by %d)\n", err)
				}
			}
			return
		} else {
			fmt.Printf("Server says - PIN is good! You really are " + IDstr)
			fmt.Printf("\n")
		}

		if FULL {
			H := BN254.MPIN_HASH_ALL(sha, HCID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BN254.MPIN_CLIENT_KEY(sha, G1[:], G2[:], pin, R[:], X[:], H[:], T[:], CK[:])
			fmt.Printf("Client Key =  0x")
			printBinary(CK[:])

			H = BN254.MPIN_HASH_ALL(sha, HSID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BN254.MPIN_SERVER_KEY(sha, Z[:], SST[:], W[:], H[:], pHID, pxID, pxCID, SK[:])
			fmt.Printf("Server Key =  0x")
			printBinary(SK[:])
		}
	}
}

func mpin_BLS12383(rng *core.RAND) {

	var sha = BLS12383.HASH_TYPE

	const MGS = BLS12383.MGS
	const MFS = BLS12383.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 4 * MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var PERMIT [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var xCID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var E [12 * MFS]byte
	var F [12 * MFS]byte
	var HID [G1S]byte
	var HTID [G1S]byte

	var G1 [12 * MFS]byte
	var G2 [12 * MFS]byte
	var R [MGS]byte
	var Z [G1S]byte
	var W [MGS]byte
	var T [G1S]byte
	var CK [BLS12383.AESKEY]byte
	var SK [BLS12383.AESKEY]byte

	var HSID []byte

	/* Trusted Authority set-up */

	fmt.Printf("\nTesting MPIN curve BLS12383\n")
	BLS12383.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

	/* Create Client Identity */
	IDstr := "testUser@miracl.com"
	CLIENT_ID := []byte(IDstr)

	HCID := BLS12383.MPIN_HASH_ID(sha, CLIENT_ID) /* Either Client or TA calculates Hash(ID) - you decide! */

	fmt.Printf("Client ID= ")
	printBinary(CLIENT_ID)
	//fmt.Printf("\n")

	/* Client and Server are issued secrets by DTA */
	BLS12383.MPIN_GET_SERVER_SECRET(S[:], SST[:])
	fmt.Printf("Server Secret SS: 0x")
	printBinary(SST[:])

	BLS12383.MPIN_GET_CLIENT_SECRET(S[:], HCID, TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])

	// Client extracts PIN from secret to create Token 

	pin := 1234
	fmt.Printf("Client extracts PIN= %d", pin)
	fmt.Printf("\n")
	rtn := BLS12383.MPIN_EXTRACT_PIN(sha, CLIENT_ID, pin, TOKEN[:])
	if rtn != 0 {
		fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
		fmt.Printf("\n")
	}

	fmt.Printf("Client Token TK: 0x")
	printBinary(TOKEN[:])

	if FULL {
		BLS12383.MPIN_PRECOMPUTE(TOKEN[:], HCID, G1[:], G2[:])
	}

	date := 0
	if PERMITS {
		date = BLS12383.Today()
		// Client gets "Time Token" permit from DTA 
		BLS12383.MPIN_GET_CLIENT_PERMIT(sha, date, S[:], HCID, PERMIT[:])
		fmt.Printf("Time Permit TP: 0x")
		printBinary(PERMIT[:])

		// This encoding makes Time permit look random - Elligator squared 
		BLS12383.MPIN_ENCODING(rng, PERMIT[:])
		fmt.Printf("Encoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
		BLS12383.MPIN_DECODING(PERMIT[:])
		fmt.Printf("Decoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
	}
	pin = 0
	fmt.Printf("\nPIN= ")
	fmt.Scanf("%d ", &pin)

	pxID := xID[:]
	pxCID := xCID[:]
	pHID := HID[:]
	pHTID := HTID[:]
	pE := E[:]
	pF := F[:]
	pPERMIT := PERMIT[:]
	var prHID []byte

	if date != 0 {
		prHID = pHTID
		if !PINERROR {
			pxID = nil
			// pHID=nil
		}
	} else {
		prHID = pHID
		pPERMIT = nil
		pxCID = nil
		pHTID = nil
	}
	if !PINERROR {
		pE = nil
		pF = nil
	}

	if SINGLE_PASS {
		fmt.Printf("MPIN Single Pass\n")
		timeValue := BLS12383.MPIN_GET_TIME()
		rtn = BLS12383.MPIN_CLIENT(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT, timeValue, Y[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS12383.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS12383.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) // Also Send Z=r.ID to Server, remember random r 
		}

		rtn = BLS12383.MPIN_SERVER(sha, date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF, CLIENT_ID, timeValue)
		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
		}

		if FULL {
			HSID = BLS12383.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS12383.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) // Also send T=w.ID to client, remember random w  
		}
	} else {
		fmt.Printf("MPIN Multi Pass\n")
		// Send U=x.ID to server, and recreate secret from token and pin 
		rtn = BLS12383.MPIN_CLIENT_1(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT)
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS12383.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS12383.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) // Also Send Z=r.ID to Server, remember random r 
		}

		// Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp. 
		BLS12383.MPIN_SERVER_1(sha, date, CLIENT_ID, pHID, pHTID)

		// Server generates Random number Y and sends it to Client 
		BLS12383.MPIN_RANDOM_GENERATE(rng, Y[:])

		if FULL {
			HSID = BLS12383.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS12383.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) // Also send T=w.ID to client, remember random w  
		}

		// Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC 
		rtn = BLS12383.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
		}

		// Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. 
		// If PIN error not required, set E and F = null 

		rtn = BLS12383.MPIN_SERVER_2(date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF)

		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER_1 rtn: %d\n", rtn)
		}

		if rtn == BLS12383.BAD_PIN {
			fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
			if PINERROR {
				err := BLS12383.MPIN_KANGAROO(E[:], F[:])
				if err != 0 {
					fmt.Printf("(Client PIN is out by %d)\n", err)
				}
			}
			return
		} else {
			fmt.Printf("Server says - PIN is good! You really are " + IDstr)
			fmt.Printf("\n")
		}

		if FULL {
			H := BLS12383.MPIN_HASH_ALL(sha, HCID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS12383.MPIN_CLIENT_KEY(sha, G1[:], G2[:], pin, R[:], X[:], H[:], T[:], CK[:])
			fmt.Printf("Client Key =  0x")
			printBinary(CK[:])

			H = BLS12383.MPIN_HASH_ALL(sha, HSID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS12383.MPIN_SERVER_KEY(sha, Z[:], SST[:], W[:], H[:], pHID, pxID, pxCID, SK[:])
			fmt.Printf("Server Key =  0x")
			printBinary(SK[:])
		}
	} 
}

func mpin_BLS24479(rng *core.RAND) {

	var sha = BLS24479.HASH_TYPE

	const MGS = BLS24479.MGS
	const MFS = BLS24479.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 8 * MFS + 1   /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var PERMIT [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var xCID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var E [24 * MFS]byte
	var F [24 * MFS]byte
	var HID [G1S]byte
	var HTID [G1S]byte

	var G1 [24 * MFS]byte
	var G2 [24 * MFS]byte
	var R [MGS]byte
	var Z [G1S]byte
	var W [MGS]byte
	var T [G1S]byte
	var CK [BLS24479.AESKEY]byte
	var SK [BLS24479.AESKEY]byte

	var HSID []byte

	/* Trusted Authority set-up */

	fmt.Printf("\nTesting MPIN curve BLS24479\n")
	BLS24479.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

	/* Create Client Identity */
	IDstr := "testUser@miracl.com"
	CLIENT_ID := []byte(IDstr)

	HCID := BLS24479.MPIN_HASH_ID(sha, CLIENT_ID) /* Either Client or TA calculates Hash(ID) - you decide! */

	fmt.Printf("Client ID= ")
	printBinary(CLIENT_ID)
	//fmt.Printf("\n")

	/* Client and Server are issued secrets by DTA */
	BLS24479.MPIN_GET_SERVER_SECRET(S[:], SST[:])
	fmt.Printf("Server Secret SS: 0x")
	printBinary(SST[:])

	BLS24479.MPIN_GET_CLIENT_SECRET(S[:], HCID, TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])

	/* Client extracts PIN from secret to create Token */
	pin := 1234
	fmt.Printf("Client extracts PIN= %d", pin)
	fmt.Printf("\n")
	rtn := BLS24479.MPIN_EXTRACT_PIN(sha, CLIENT_ID, pin, TOKEN[:])
	if rtn != 0 {
		fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
		fmt.Printf("\n")
	}

	fmt.Printf("Client Token TK: 0x")
	printBinary(TOKEN[:])

	if FULL {
		BLS24479.MPIN_PRECOMPUTE(TOKEN[:], HCID, G1[:], G2[:])
	}

	date := 0
	if PERMITS {
		date = BLS24479.Today()
		/* Client gets "Time Token" permit from DTA */
		BLS24479.MPIN_GET_CLIENT_PERMIT(sha, date, S[:], HCID, PERMIT[:])
		fmt.Printf("Time Permit TP: 0x")
		printBinary(PERMIT[:])

		/* This encoding makes Time permit look random - Elligator squared */
		BLS24479.MPIN_ENCODING(rng, PERMIT[:])
		fmt.Printf("Encoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
		BLS24479.MPIN_DECODING(PERMIT[:])
		fmt.Printf("Decoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
	}
	pin = 0
	fmt.Printf("\nPIN= ")
	fmt.Scanf("%d ", &pin)

	pxID := xID[:]
	pxCID := xCID[:]
	pHID := HID[:]
	pHTID := HTID[:]
	pE := E[:]
	pF := F[:]
	pPERMIT := PERMIT[:]
	var prHID []byte

	if date != 0 {
		prHID = pHTID
		if !PINERROR {
			pxID = nil
			// pHID=nil
		}
	} else {
		prHID = pHID
		pPERMIT = nil
		pxCID = nil
		pHTID = nil
	}
	if !PINERROR {
		pE = nil
		pF = nil
	}

	if SINGLE_PASS {
		fmt.Printf("MPIN Single Pass\n")
		timeValue := BLS24479.MPIN_GET_TIME()
		rtn = BLS24479.MPIN_CLIENT(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT, timeValue, Y[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS24479.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS24479.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		rtn = BLS24479.MPIN_SERVER(sha, date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF, CLIENT_ID, timeValue)
		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
		}

		if FULL {
			HSID = BLS24479.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS24479.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}
	} else {
		fmt.Printf("MPIN Multi Pass\n")
		/* Send U=x.ID to server, and recreate secret from token and pin */
		rtn = BLS24479.MPIN_CLIENT_1(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT)
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS24479.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS24479.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		/* Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp. */
		BLS24479.MPIN_SERVER_1(sha, date, CLIENT_ID, pHID, pHTID)

		/* Server generates Random number Y and sends it to Client */
		BLS24479.MPIN_RANDOM_GENERATE(rng, Y[:])

		if FULL {
			HSID = BLS24479.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS24479.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}

		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
		rtn = BLS24479.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
		}

		/* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
		/* If PIN error not required, set E and F = null */

		rtn = BLS24479.MPIN_SERVER_2(date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF)

		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER_1 rtn: %d\n", rtn)
		}

		if rtn == BLS24479.BAD_PIN {
			fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
			if PINERROR {
				err := BLS24479.MPIN_KANGAROO(E[:], F[:])
				if err != 0 {
					fmt.Printf("(Client PIN is out by %d)\n", err)
				}
			}
			return
		} else {
			fmt.Printf("Server says - PIN is good! You really are " + IDstr)
			fmt.Printf("\n")
		}

		if FULL {
			H := BLS24479.MPIN_HASH_ALL(sha, HCID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS24479.MPIN_CLIENT_KEY(sha, G1[:], G2[:], pin, R[:], X[:], H[:], T[:], CK[:])
			fmt.Printf("Client Key =  0x")
			printBinary(CK[:])

			H = BLS24479.MPIN_HASH_ALL(sha, HSID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS24479.MPIN_SERVER_KEY(sha, Z[:], SST[:], W[:], H[:], pHID, pxID, pxCID, SK[:])
			fmt.Printf("Server Key =  0x")
			printBinary(SK[:])
		}
	}
}

func mpin_BLS48556(rng *core.RAND) {

	var sha = BLS48556.HASH_TYPE

	const MGS = BLS48556.MGS
	const MFS = BLS48556.MFS
	const G1S = 2*MFS + 1 /* Group 1 Size */
	const G2S = 16 * MFS + 1  /* Group 2 Size */

	var S [MGS]byte
	var SST [G2S]byte
	var TOKEN [G1S]byte
	var PERMIT [G1S]byte
	var SEC [G1S]byte
	var xID [G1S]byte
	var xCID [G1S]byte
	var X [MGS]byte
	var Y [MGS]byte
	var E [48 * MFS]byte
	var F [48 * MFS]byte
	var HID [G1S]byte
	var HTID [G1S]byte

	var G1 [48 * MFS]byte
	var G2 [48 * MFS]byte
	var R [MGS]byte
	var Z [G1S]byte
	var W [MGS]byte
	var T [G1S]byte
	var CK [BLS48556.AESKEY]byte
	var SK [BLS48556.AESKEY]byte

	var HSID []byte

	/* Trusted Authority set-up */

	fmt.Printf("\nTesting MPIN curve BLS48556\n")
	BLS48556.MPIN_RANDOM_GENERATE(rng, S[:])
	fmt.Printf("Master Secret s: 0x")
	printBinary(S[:])

	/* Create Client Identity */
	IDstr := "testUser@miracl.com"
	CLIENT_ID := []byte(IDstr)

	HCID := BLS48556.MPIN_HASH_ID(sha, CLIENT_ID) /* Either Client or TA calculates Hash(ID) - you decide! */

	fmt.Printf("Client ID= ")
	printBinary(CLIENT_ID)
	//fmt.Printf("\n")

	/* Client and Server are issued secrets by DTA */
	BLS48556.MPIN_GET_SERVER_SECRET(S[:], SST[:])
	fmt.Printf("Server Secret SS: 0x")
	printBinary(SST[:])

	BLS48556.MPIN_GET_CLIENT_SECRET(S[:], HCID, TOKEN[:])
	fmt.Printf("Client Secret CS: 0x")
	printBinary(TOKEN[:])

	/* Client extracts PIN from secret to create Token */
	pin := 1234
	fmt.Printf("Client extracts PIN= %d", pin)
	fmt.Printf("\n")
	rtn := BLS48556.MPIN_EXTRACT_PIN(sha, CLIENT_ID, pin, TOKEN[:])
	if rtn != 0 {
		fmt.Printf("FAILURE: EXTRACT_PIN rtn: %d", rtn)
		fmt.Printf("\n")
	}

	fmt.Printf("Client Token TK: 0x")
	printBinary(TOKEN[:])

	if FULL {
		BLS48556.MPIN_PRECOMPUTE(TOKEN[:], HCID, G1[:], G2[:])
	}

	date := 0
	if PERMITS {
		date = BLS48556.Today()
		/* Client gets "Time Token" permit from DTA */
		BLS48556.MPIN_GET_CLIENT_PERMIT(sha, date, S[:], HCID, PERMIT[:])
		fmt.Printf("Time Permit TP: 0x")
		printBinary(PERMIT[:])

		/* This encoding makes Time permit look random - Elligator squared */
		BLS48556.MPIN_ENCODING(rng, PERMIT[:])
		fmt.Printf("Encoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
		BLS48556.MPIN_DECODING(PERMIT[:])
		fmt.Printf("Decoded Time Permit TP: 0x")
		printBinary(PERMIT[:])
	}
	pin = 0
	fmt.Printf("\nPIN= ")
	fmt.Scanf("%d ", &pin)

	pxID := xID[:]
	pxCID := xCID[:]
	pHID := HID[:]
	pHTID := HTID[:]
	pE := E[:]
	pF := F[:]
	pPERMIT := PERMIT[:]
	var prHID []byte

	if date != 0 {
		prHID = pHTID
		if !PINERROR {
			pxID = nil
			// pHID=nil
		}
	} else {
		prHID = pHID
		pPERMIT = nil
		pxCID = nil
		pHTID = nil
	}
	if !PINERROR {
		pE = nil
		pF = nil
	}

	if SINGLE_PASS {
		fmt.Printf("MPIN Single Pass\n")
		timeValue := BLS48556.MPIN_GET_TIME()
		rtn = BLS48556.MPIN_CLIENT(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT, timeValue, Y[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS48556.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS48556.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		rtn = BLS48556.MPIN_SERVER(sha, date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF, CLIENT_ID, timeValue)
		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER rtn: %d\n", rtn)
		}

		if FULL {
			HSID = BLS48556.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS48556.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}
	} else {
		fmt.Printf("MPIN Multi Pass\n")
		/* Send U=x.ID to server, and recreate secret from token and pin */
		rtn = BLS48556.MPIN_CLIENT_1(sha, date, CLIENT_ID, rng, X[:], pin, TOKEN[:], SEC[:], pxID, pxCID, pPERMIT)
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_1 rtn: %d\n", rtn)
		}

		if FULL {
			HCID = BLS48556.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS48556.MPIN_GET_G1_MULTIPLE(rng, 1, R[:], HCID, Z[:]) /* Also Send Z=r.ID to Server, remember random r */
		}

		/* Server calculates H(ID) and H(T|H(ID)) (if time permits enabled), and maps them to points on the curve HID and HTID resp. */
		BLS48556.MPIN_SERVER_1(sha, date, CLIENT_ID, pHID, pHTID)

		/* Server generates Random number Y and sends it to Client */
		BLS48556.MPIN_RANDOM_GENERATE(rng, Y[:])

		if FULL {
			HSID = BLS48556.MPIN_HASH_ID(sha, CLIENT_ID)
			BLS48556.MPIN_GET_G1_MULTIPLE(rng, 0, W[:], prHID, T[:]) /* Also send T=w.ID to client, remember random w  */
		}

		/* Client Second Pass: Inputs Client secret SEC, x and y. Outputs -(x+y)*SEC */
		rtn = BLS48556.MPIN_CLIENT_2(X[:], Y[:], SEC[:])
		if rtn != 0 {
			fmt.Printf("FAILURE: CLIENT_2 rtn: %d\n", rtn)
		}

		/* Server Second pass. Inputs hashed client id, random Y, -(x+y)*SEC, xID and xCID and Server secret SST. E and F help kangaroos to find error. */
		/* If PIN error not required, set E and F = null */

		rtn = BLS48556.MPIN_SERVER_2(date, pHID, pHTID, Y[:], SST[:], pxID, pxCID, SEC[:], pE, pF)

		if rtn != 0 {
			fmt.Printf("FAILURE: SERVER_2 rtn: %d\n", rtn)
		}

		if rtn == BLS48556.BAD_PIN {
			fmt.Printf("Server says - Bad Pin. I don't know you. Feck off.\n")
			if PINERROR {
				err := BLS48556.MPIN_KANGAROO(E[:], F[:])
				if err != 0 {
					fmt.Printf("(Client PIN is out by %d)\n", err)
				}
			}
			return
		} else {
			fmt.Printf("Server says - PIN is good! You really are " + IDstr)
			fmt.Printf("\n")
		}

		if FULL {
			H := BLS48556.MPIN_HASH_ALL(sha, HCID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS48556.MPIN_CLIENT_KEY(sha, G1[:], G2[:], pin, R[:], X[:], H[:], T[:], CK[:])
			fmt.Printf("Client Key =  0x")
			printBinary(CK[:])

			H = BLS48556.MPIN_HASH_ALL(sha, HSID[:], pxID, pxCID, SEC[:], Y[:], Z[:], T[:])
			BLS48556.MPIN_SERVER_KEY(sha, Z[:], SST[:], W[:], H[:], pHID, pxID, pxCID, SK[:])
			fmt.Printf("Server Key =  0x")
			printBinary(SK[:])
		}
	}
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
