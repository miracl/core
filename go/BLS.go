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

/* Boneh-Lynn-Shacham  API Functions */

package XXX

import "github.com/miracl/core/go/core"

//import "fmt"

const BFS int = int(MODBYTES)
const BGS int = int(MODBYTES)
const BLS_OK int = 0
const BLS_FAIL int = -1

var G2_TAB []*FP4

/* hash a message to an ECP point, using SHA3 */

func bls_hash(m string) *ECP {
	sh := core.NewSHA3(core.SHA3_SHAKE256)
	var hm [BFS]byte
	t := []byte(m)
	for i := 0; i < len(t); i++ {
		sh.Process(t[i])
	}
	sh.Shake(hm[:], BFS)
	P := ECP_mapit(hm[:])
	return P
}

func Init() int {
	G := ECP2_generator()
	if G.Is_infinity() {
		return BLS_FAIL
	}
	G2_TAB = precomp(G)
	return BLS_OK
}

/* generate key pair, private key S, public key W */

func KeyPairGenerate(rng *core.RAND, S []byte, W []byte) int {
	G := ECP2_generator()
	if G.Is_infinity() {
		return BLS_FAIL
	}
	q := NewBIGints(CURVE_Order)
	s := Randtrunc(q, 16*AESKEY, rng)
	s.ToBytes(S)
	G = G2mul(G, s)
	G.ToBytes(W,true)
	return BLS_OK
}

/* Sign message m using private key S to produce signature SIG */

func Sign(SIG []byte, m string, S []byte) int {
	D := bls_hash(m)
	s := FromBytes(S)
	D = G1mul(D, s)
	D.ToBytes(SIG, true)
	return BLS_OK
}

/* Verify signature given message m, the signature SIG, and the public key W */

func Verify(SIG []byte, m string, W []byte) int {
	HM := bls_hash(m)
	
	D := ECP_fromBytes(SIG)
	if !G1member(D) {return BLS_FAIL}
	D.neg()

	PK := ECP2_fromBytes(W)

	// Use new multi-pairing mechanism

	r := initmp()
	another_pc(r, G2_TAB, D)
	another(r, PK, HM)
	v := miller(r)

	//.. or alternatively
	//	G := ECP2_generator()
	//	if G.Is_infinity() {return BLS_FAIL}
	//	v := Ate2(G, D, PK, HM)

	v = Fexp(v)

	if v.Isunity() {
		return BLS_OK
	} else {
		return BLS_FAIL
	}
}
