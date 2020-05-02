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

import "github.com/miracl/core/go/core"

//import "core"

func main() {

	fmt.Printf("\nTesting New Hope Key Exchange\n")

	srng := core.NewRAND()
	var sraw [100]byte
	for i := 0; i < 100; i++ {
		sraw[i] = byte(i + 1)
	}
	srng.Seed(100, sraw[:])

	crng := core.NewRAND()
	var craw [100]byte
	for i := 0; i < 100; i++ {
		craw[i] = byte(i + 2)
	}
	crng.Seed(100, craw[:])

	var S [1792]byte

	var SB [1824]byte
	core.NHS_SERVER_1(srng, SB[:], S[:])
	var UC [2176]byte
	var KEYB [32]byte
	core.NHS_CLIENT(crng, SB[:], UC[:], KEYB[:])

	fmt.Printf("Bob's Key= ")
	for i := 0; i < 32; i++ {
		fmt.Printf("%02x", KEYB[i])
	}
	fmt.Printf("\n")
	var KEYA [32]byte
	core.NHS_SERVER_2(S[:], UC[:], KEYA[:])

	fmt.Printf("Alice Key= ")
	for i := 0; i < 32; i++ {
		fmt.Printf("%02x", KEYA[i])
	}
	fmt.Printf("\n")

}
