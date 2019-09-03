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
