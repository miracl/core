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

/* test driver and function exerciser for NewHope Simple Functions */
// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

package main

import "fmt"

import "miracl/core"

//import "core"

func main() {
	var raw [100]byte
	var r32 [32]byte
	var r64 [64]byte
	var SK [core.KYBER_SECRET_CCA_SIZE_768]byte
	var PK [core.KYBER_PUBLIC_SIZE_768]byte
	var SS [core.KYBER_SHARED_SECRET_768]byte
	var CT [core.KYBER_CIPHERTEXT_SIZE_768]byte	

	loops:=100

	fmt.Printf("\nTesting Kyber Key Exchange\n")
	rng := core.NewRAND()

	for i := 0; i < 100; i++ {
		raw[i] = byte(i + 1)
	}
	rng.Seed(100, raw[:])

	for j:=0;j<loops;j++ {
		for i := 0; i < 64; i++ {
			r64[i] = rng.GetByte()
		}

		core.KYBER_keypair768(r64[:],SK[:],PK[:])
/*
		fmt.Printf("Secret Key= 0x")
		for i := 0; i < len(SK); i++ {
			fmt.Printf("%02x", SK[i])
		}
		fmt.Printf("\n")
		fmt.Printf("Public Key= 0x")
		for i := 0; i < len(PK); i++ {
			fmt.Printf("%02x", PK[i])
		}
		fmt.Printf("\n")
 */
		for i := 0; i < 32; i++ {
			r32[i] = rng.GetByte()
		}

		core.KYBER_encrypt768(r32[:],PK[:],SS[:],CT[:])

/*
		fmt.Printf("Ciphertext= 0x")
		for i := 0; i < len(CT); i++ {
			fmt.Printf("%02x", CT[i])
		}
		fmt.Printf("\n")
*/
		fmt.Printf("j= %d\n",j)
		for i := 0; i < len(SS); i++ {
			fmt.Printf("%02x", SS[i])
		}
		fmt.Printf("\n")

		core.KYBER_decrypt768(SK[:],CT[:],SS[:])

		for i := 0; i < len(SS); i++ {
			fmt.Printf("%02x", SS[i])
		}
		fmt.Printf("\n")
	}
}
