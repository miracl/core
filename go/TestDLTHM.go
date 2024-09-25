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

	Arwa Alblooshi 15/12/2022
 */
 
package main

import "fmt"
import "miracl/core"

func main() {
	LOOPS := 100
	var attempts int
	var result bool

	var RAW [100]byte
	var TAU [32]byte
	var SK [core.DL_SK_SIZE_3]byte
	var PK [core.DL_PK_SIZE_3]byte
	var SIG [core.DL_SIG_SIZE_3]byte
	//var CT [core.CIPHERTEXT_SIZE_768]byte
	//var M [128]byte
	
	RNG := core.NewRAND()

	for i := 0; i < 100; i++ {
		RAW[i] = byte(i + 1)
	}
	RNG.Seed(100,RAW[:])

	fmt.Printf("\nTesting Dilithium Signature\n")

	tats := 0
	for j := 0; j < LOOPS; j++ {
		mess := "Hello World"
		for i := 0; i < 32; i++ {
			TAU[i] = byte(RNG.GetByte()) //byte(i+1)
		}

		core.DL_keypair_3(TAU[:], SK[:], PK[:])
		/*for i := 0; i < len(PK); i++ {
			fmt.Printf("%02x", PK[i])
		}*/

		/*fmt.Printf("Secret key = 0x")
		for i := 0; i < len(SK); i++ {
			fmt.Printf("%02x", SK[i])
		}*/

		attempts = core.DL_signature_3(false, nil, SK[:], nil, []byte(mess), SIG[:])
		tats += attempts
		/* fmt.Println("Signature = 0x")
		for i:=0;i<len(SIG);i++{
			fmt.Println("%20x", SIG[i])
		}
		*/

		fmt.Printf("Signature %d bits created on attempt %d\n", 8*len(SIG[:]), attempts)

		result = core.DL_verify_3(false, PK[:], nil, []byte(mess), SIG[:])
		if result {
			fmt.Println("Signature is verified")
		} else {
			fmt.Println("Signature is NOT verified")
			break
		}
	}
	if LOOPS > 1 {
		fmt.Printf("Average = %d\n", tats/LOOPS)
	}
}
