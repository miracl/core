/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/* test driver and function exerciser for BLS API Functions */

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
import "github.com/miracl/core/go/core/BLS383"
import "github.com/miracl/core/go/core/BLS24"
import "github.com/miracl/core/go/core/BLS48"

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

    for i:=0;i<32;i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
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

func bls_BLS383(rng *core.RAND) {

	const BGS = BLS383.BGS
	const BFS = BLS383.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 2 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [32]byte

    for i:=0;i<32;i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	res := BLS383.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS383.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS383.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS383.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS24(rng *core.RAND) {

	const BGS = BLS24.BGS
	const BFS = BLS24.BFS
	const G1S = BFS + 1 /* Group 1 Size */
	const G2S = 4 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [48]byte

    for i:=0;i<48;i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	res := BLS24.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS24.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS24.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS24.Core_Verify(SIG[:], []byte(mess), W[:])

	if res == 0 {
		fmt.Printf("Signature is OK\n")
	} else {
		fmt.Printf("Signature is *NOT* OK\n")
	}
}

func bls_BLS48(rng *core.RAND) {

	const BGS = BLS48.BGS
	const BFS = BLS48.BFS
	const G1S = BFS + 1  /* Group 1 Size */
	const G2S = 8 * BFS + 1 /* Group 2 Size */

	var S [BGS]byte
	var W [G2S]byte
	var SIG [G1S]byte
	var IKM [64]byte

    for i:=0;i<64;i++ {
		//IKM[i] = byte(i+1)
        IKM[i]=byte(rng.GetByte())
	}

	fmt.Printf("\nTesting Boneh-Lynn-Shacham BLS signature code\n")
	mess := "This is a test message"

	res := BLS48.Init()
	if res != 0 {
		fmt.Printf("Failed to Initialize\n")
		return
	}

	res = BLS48.KeyPairGenerate(IKM[:], S[:], W[:])
	if res != 0 {
		fmt.Printf("Failed to generate keys\n")
		return
	}
	fmt.Printf("Private key : 0x")
	printBinary(S[:])
	fmt.Printf("Public  key : 0x")
	printBinary(W[:])

	BLS48.Core_Sign(SIG[:], []byte(mess), S[:])
	fmt.Printf("Signature : 0x")
	printBinary(SIG[:])

	res = BLS48.Core_Verify(SIG[:], []byte(mess), W[:])

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
	bls_BLS383(rng)
	bls_BLS24(rng)
	bls_BLS48(rng)
}
