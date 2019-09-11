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

/* Test and benchmark elliptic curve and RSA functions */

package main

import "fmt"

import "github.com/miracl/core/go/core"
import "github.com/miracl/core/go/core/ED25519"
import "github.com/miracl/core/go/core/NIST256"
import "github.com/miracl/core/go/core/GOLDILOCKS"
import "github.com/miracl/core/go/core/BN254"
import "github.com/miracl/core/go/core/BLS383"
import "github.com/miracl/core/go/core/RSA2048"
import "github.com/miracl/core/go/core/BLS24"
import "github.com/miracl/core/go/core/BLS48"

//import "core"
//import "core/BN254"
//import "core/ED25519"
//import "core/RSA2048"

import "time"

const MIN_TIME int = 10
const MIN_ITERS int = 10

func ED_25519(rng *core.RAND) {
	fmt.Printf("\nTesting/Timing ED25519 ECC\n")

	if ED25519.CURVETYPE == ED25519.WEIERSTRASS {
		fmt.Printf("Weierstrass parameterization\n")
	}
	if ED25519.CURVETYPE == ED25519.EDWARDS {
		fmt.Printf("Edwards parameterization\n")
	}
	if ED25519.CURVETYPE == ED25519.MONTGOMERY {
		fmt.Printf("Montgomery parameterization\n")
	}

	if ED25519.MODTYPE == ED25519.PSEUDO_MERSENNE {
		fmt.Printf("Pseudo-Mersenne Modulus\n")
	}
	if ED25519.MODTYPE == ED25519.MONTGOMERY_FRIENDLY {
		fmt.Printf("Montgomery friendly Modulus\n")
	}
	if ED25519.MODTYPE == ED25519.GENERALISED_MERSENNE {
		fmt.Printf("Generalised-Mersenne Modulus\n")
	}
	if ED25519.MODTYPE == ED25519.NOT_SPECIAL {
		fmt.Printf("Not special Modulus\n")
	}

	fmt.Printf("Modulus size %d bits\n", ED25519.MODBITS)
	fmt.Printf("%d bit build\n", ED25519.CHUNK)

	var es *ED25519.BIG

	EG := ED25519.ECP_generator()
	er := ED25519.NewBIGints(ED25519.CURVE_Order)
	es = ED25519.Randtrunc(er, 16*ED25519.AESKEY, rng)

	WP:=ED25519.ECP_hashit(es)
	WP.Cfp();
    if (WP.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	WP = EG.Mul(er)
	if !WP.Is_infinity() {
		fmt.Printf("FAILURE - rG!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		WP = EG.Mul(es)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("EC  mul - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

}

func NIST_256(rng *core.RAND) {
	fmt.Printf("\nTesting/Timing NIST256 ECC\n")

	if NIST256.CURVETYPE == NIST256.WEIERSTRASS {
		fmt.Printf("Weierstrass parameterization\n")
	}
	if NIST256.CURVETYPE == NIST256.EDWARDS {
		fmt.Printf("Edwards parameterization\n")
	}
	if NIST256.CURVETYPE == NIST256.MONTGOMERY {
		fmt.Printf("Montgomery parameterization\n")
	}

	if NIST256.MODTYPE == NIST256.PSEUDO_MERSENNE {
		fmt.Printf("Pseudo-Mersenne Modulus\n")
	}
	if NIST256.MODTYPE == NIST256.MONTGOMERY_FRIENDLY {
		fmt.Printf("Montgomery friendly Modulus\n")
	}
	if NIST256.MODTYPE == NIST256.GENERALISED_MERSENNE {
		fmt.Printf("Generalised-Mersenne Modulus\n")
	}
	if NIST256.MODTYPE == NIST256.NOT_SPECIAL {
		fmt.Printf("Not special Modulus\n")
	}

	fmt.Printf("Modulus size %d bits\n", NIST256.MODBITS)
	fmt.Printf("%d bit build\n", NIST256.CHUNK)

	var es *NIST256.BIG

	EG := NIST256.ECP_generator()
	er := NIST256.NewBIGints(NIST256.CURVE_Order)
	es = NIST256.Randtrunc(er, 16*NIST256.AESKEY, rng)

	WP:=NIST256.ECP_hashit(es)
	WP.Cfp();
    if (WP.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	WP = EG.Mul(er)
	if !WP.Is_infinity() {
		fmt.Printf("FAILURE - rG!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		WP = EG.Mul(es)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("EC  mul - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

}

func GOLDI_LOCKS(rng *core.RAND) {
	fmt.Printf("\nTesting/Timing GOLDILOCKS ECC\n")

	if GOLDILOCKS.CURVETYPE == GOLDILOCKS.WEIERSTRASS {
		fmt.Printf("Weierstrass parameterization\n")
	}
	if GOLDILOCKS.CURVETYPE == GOLDILOCKS.EDWARDS {
		fmt.Printf("Edwards parameterization\n")
	}
	if GOLDILOCKS.CURVETYPE == GOLDILOCKS.MONTGOMERY {
		fmt.Printf("Montgomery parameterization\n")
	}

	if GOLDILOCKS.MODTYPE == GOLDILOCKS.PSEUDO_MERSENNE {
		fmt.Printf("Pseudo-Mersenne Modulus\n")
	}
	if GOLDILOCKS.MODTYPE == GOLDILOCKS.MONTGOMERY_FRIENDLY {
		fmt.Printf("Montgomery friendly Modulus\n")
	}
	if GOLDILOCKS.MODTYPE == GOLDILOCKS.GENERALISED_MERSENNE {
		fmt.Printf("Generalised-Mersenne Modulus\n")
	}
	if GOLDILOCKS.MODTYPE == GOLDILOCKS.NOT_SPECIAL {
		fmt.Printf("Not special Modulus\n")
	}

	fmt.Printf("Modulus size %d bits\n", GOLDILOCKS.MODBITS)
	fmt.Printf("%d bit build\n", GOLDILOCKS.CHUNK)

	var es *GOLDILOCKS.BIG

	EG := GOLDILOCKS.ECP_generator()
	er := GOLDILOCKS.NewBIGints(GOLDILOCKS.CURVE_Order)
	es = GOLDILOCKS.Randtrunc(er, 16*GOLDILOCKS.AESKEY, rng)

	WP:=GOLDILOCKS.ECP_hashit(es)
	WP.Cfp();
    if (WP.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	WP = EG.Mul(er)
	if !WP.Is_infinity() {
		fmt.Printf("FAILURE - rG!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		WP = EG.Mul(es)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("EC  mul - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

}

func BN_254(rng *core.RAND) {

	fmt.Printf("\nTesting/Timing BN254 Pairings\n")

	if BN254.CURVE_PAIRING_TYPE == BN254.BN {
		fmt.Printf("BN Pairing-Friendly Curve\n")
	}
	if BN254.CURVE_PAIRING_TYPE == BN254.BLS {
		fmt.Printf("BLS Pairing-Friendly Curve\n")
	}

	fmt.Printf("Modulus size %d bits\n", BN254.MODBITS)
	fmt.Printf("%d bit build\n", BN254.CHUNK)

	G := BN254.ECP_generator()
	r := BN254.NewBIGints(BN254.CURVE_Order)
	s := BN254.Randtrunc(r, 16*BN254.AESKEY, rng)

	P:=BN254.ECP_hashit(s)
	P.Cfp();
    if (P.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	P = BN254.G1mul(G, r)

	if !P.Is_infinity() {
		fmt.Printf("FAILURE - rP!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		P = BN254.G1mul(G, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G1 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	Q := BN254.ECP2_generator()
	W := BN254.G2mul(Q, r)

	if !W.Is_infinity() {
		fmt.Printf("FAILURE - rQ!=O\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		W = BN254.G2mul(Q, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G2 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	w := BN254.Ate(Q, P)
	w = BN254.Fexp(w)

	g := BN254.GTpow(w, r)

	if !g.Isunity() {
		fmt.Printf("FAILURE - g^r!=1\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BN254.GTpow(w, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		_ = w.Compow(s, r)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow (compressed) - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		w = BN254.Ate(Q, P)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing ATE         - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BN254.Fexp(w)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing FEXP        - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	P.Copy(G)
	Q.Copy(W)

	P = BN254.G1mul(P, s)

	g = BN254.Ate(Q, P)
	g = BN254.Fexp(g)

	P.Copy(G)
	Q = BN254.G2mul(Q, s)

	w = BN254.Ate(Q, P)
	w = BN254.Fexp(w)

	if !BN254.G1member(P) {
		fmt.Printf("FAILURE - P not in G1 \n")
		return
	}

	if !BN254.G2member(Q) {
		fmt.Printf("FAILURE - Q not in G2 \n")
		return
	}

	if !BN254.GTmember(w) {
		fmt.Printf("FAILURE - e(Q,P) not in GT \n")
		return
	}


	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,sP) \n")
		return
	}

	Q.Copy(W)
	g = BN254.Ate(Q, P)
	g = BN254.Fexp(g)
	g = BN254.GTpow(g, s)

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n")
		return
	}

}

func BLS_383(rng *core.RAND) {

	fmt.Printf("\nTesting/Timing BLS383 Pairings\n")

	if BLS383.CURVE_PAIRING_TYPE == BLS383.BN {
		fmt.Printf("BN Pairing-Friendly Curve\n")
	}
	if BLS383.CURVE_PAIRING_TYPE == BLS383.BLS {
		fmt.Printf("BLS Pairing-Friendly Curve\n")
	}

	fmt.Printf("Modulus size %d bits\n", BLS383.MODBITS)
	fmt.Printf("%d bit build\n", BLS383.CHUNK)

	G := BLS383.ECP_generator()
	r := BLS383.NewBIGints(BLS383.CURVE_Order)
	s := BLS383.Randtrunc(r, 16*BLS383.AESKEY, rng)

	P:=BLS383.ECP_hashit(s)
	P.Cfp();
    if (P.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	P = BLS383.G1mul(G, r)

	if !P.Is_infinity() {
		fmt.Printf("FAILURE - rP!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		P = BLS383.G1mul(G, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G1 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	Q := BLS383.ECP2_generator()
	W := BLS383.G2mul(Q, r)

	if !W.Is_infinity() {
		fmt.Printf("FAILURE - rQ!=O\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		W = BLS383.G2mul(Q, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G2 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	w := BLS383.Ate(Q, P)
	w = BLS383.Fexp(w)

	g := BLS383.GTpow(w, r)

	if !g.Isunity() {
		fmt.Printf("FAILURE - g^r!=1\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS383.GTpow(w, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		_ = w.Compow(s, r)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow (compressed) - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		w = BLS383.Ate(Q, P)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing ATE         - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS383.Fexp(w)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing FEXP        - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	P.Copy(G)
	Q.Copy(W)

	P = BLS383.G1mul(P, s)

	g = BLS383.Ate(Q, P)
	g = BLS383.Fexp(g)

	P.Copy(G)
	Q = BLS383.G2mul(Q, s)

	w = BLS383.Ate(Q, P)
	w = BLS383.Fexp(w)

	if !BLS383.G1member(P) {
		fmt.Printf("FAILURE - P not in G1 \n")
		return
	}

	if !BLS383.G2member(Q) {
		fmt.Printf("FAILURE - Q not in G2 \n")
		return
	}

	if !BLS383.GTmember(w) {
		fmt.Printf("FAILURE - e(Q,P) not in GT \n")
		return

	}


	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,sP) \n")
		return
	}

	Q.Copy(W)
	g = BLS383.Ate(Q, P)
	g = BLS383.Fexp(g)
	g = BLS383.GTpow(g, s)

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n")
		return
	}

}

func BLS_24(rng *core.RAND) {

	fmt.Printf("\nTesting/Timing BLS24 Pairings\n")

	if BLS24.CURVE_PAIRING_TYPE == BLS24.BN {
		fmt.Printf("BN Pairing-Friendly Curve\n")
	}
	if BLS24.CURVE_PAIRING_TYPE == BLS24.BLS {
		fmt.Printf("BLS Pairing-Friendly Curve\n")
	}

	fmt.Printf("Modulus size %d bits\n", BLS24.MODBITS)
	fmt.Printf("%d bit build\n", BLS24.CHUNK)

	G := BLS24.ECP_generator()
	r := BLS24.NewBIGints(BLS24.CURVE_Order)
	s := BLS24.Randtrunc(r, 16*BLS24.AESKEY, rng)

	P:=BLS24.ECP_hashit(s)
	P.Cfp()
    if (P.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	P = BLS24.G1mul(G, r)

	if !P.Is_infinity() {
		fmt.Printf("FAILURE - rP!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		P = BLS24.G1mul(G, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G1 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	Q := BLS24.ECP4_generator()

	W := BLS24.G2mul(Q, r)

	if !W.Is_infinity() {
		fmt.Printf("FAILURE - rQ!=O\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		W = BLS24.G2mul(Q, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G2 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	w := BLS24.Ate(Q, P)
	w = BLS24.Fexp(w)

	g := BLS24.GTpow(w, r)

	if !g.Isunity() {
		fmt.Printf("FAILURE - g^r!=1\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS24.GTpow(w, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)
/*
	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		_ = w.Compow(s, r)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow (compressed) - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)
*/
	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		w = BLS24.Ate(Q, P)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing ATE         - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS24.Fexp(w)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing FEXP        - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	P.Copy(G)
	Q.Copy(W)

	P = BLS24.G1mul(P, s)

	g = BLS24.Ate(Q, P)
	g = BLS24.Fexp(g)

	P.Copy(G)
	Q = BLS24.G2mul(Q, s)

	w = BLS24.Ate(Q, P)
	w = BLS24.Fexp(w)

	if !BLS24.G1member(P) {
		fmt.Printf("FAILURE - P not in G1 \n")
		return
	}

	if !BLS24.G2member(Q) {
		fmt.Printf("FAILURE - Q not in G2 \n")
		return
	}

	if !BLS24.GTmember(w) {
		fmt.Printf("FAILURE - e(Q,P) not in GT \n")
		return

	}

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,sP) \n")
		return
	}

	Q.Copy(W)
	g = BLS24.Ate(Q, P)
	g = BLS24.Fexp(g)
	g = BLS24.GTpow(g, s)

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n")
		return
	}

}

func BLS_48(rng *core.RAND) {

	fmt.Printf("\nTesting/Timing BLS48 Pairings\n")

	if BLS48.CURVE_PAIRING_TYPE == BLS48.BN {
		fmt.Printf("BN Pairing-Friendly Curve\n")
	}
	if BLS48.CURVE_PAIRING_TYPE == BLS48.BLS {
		fmt.Printf("BLS Pairing-Friendly Curve\n")
	}

	fmt.Printf("Modulus size %d bits\n", BLS48.MODBITS)
	fmt.Printf("%d bit build\n", BLS48.CHUNK)

	G := BLS48.ECP_generator()
	r := BLS48.NewBIGints(BLS48.CURVE_Order)
	s := BLS48.Randtrunc(r, 16*BLS48.AESKEY, rng)

	P:=BLS48.ECP_hashit(s)
	P.Cfp()
    if (P.Is_infinity()) {
        fmt.Printf("HASHING FAILURE - P=O\n");
		return
    }

	P = BLS48.G1mul(G, r)

	if !P.Is_infinity() {
		fmt.Printf("FAILURE - rP!=O\n")
		return
	}

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		P = BLS48.G1mul(G, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G1 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	Q := BLS48.ECP8_generator()

	W := BLS48.G2mul(Q, r)

	if !W.Is_infinity() {
		fmt.Printf("FAILURE - rQ!=O\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		W = BLS48.G2mul(Q, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("G2 mul              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	w := BLS48.Ate(Q, P)
	w = BLS48.Fexp(w)

	g := BLS48.GTpow(w, r)

	if !g.Isunity() {
		fmt.Printf("FAILURE - g^r!=1\n")
		return
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS48.GTpow(w, s)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow              - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)
/*
	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		_ = w.Compow(s, r)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("GT pow (compressed) - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)
*/
	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		w = BLS48.Ate(Q, P)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing ATE         - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		g = BLS48.Fexp(w)
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("PAIRing FEXP        - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	P.Copy(G)
	Q.Copy(W)

	P = BLS48.G1mul(P, s)

	g = BLS48.Ate(Q, P)
	g = BLS48.Fexp(g)

	P.Copy(G)
	Q = BLS48.G2mul(Q, s)

	w = BLS48.Ate(Q, P)
	w = BLS48.Fexp(w)

	if !BLS48.G1member(P) {
		fmt.Printf("FAILURE - P not in G1 \n")
		return
	}

	if !BLS48.G2member(Q) {
		fmt.Printf("FAILURE - Q not in G2 \n")
		return
	}

	if !BLS48.GTmember(w) {
		fmt.Printf("FAILURE - e(Q,P) not in GT \n")
		return

	}

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,sP) \n")
		return
	}

	Q.Copy(W)
	g = BLS48.Ate(Q, P)
	g = BLS48.Fexp(g)
	g = BLS48.GTpow(g, s)

	if !g.Equals(w) {
		fmt.Printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n")
		return
	}

}

func RSA_2048(rng *core.RAND) {

	fmt.Printf("\nTesting/Timing 2048-bit RSA\n")

	pub := RSA2048.New_public_key(RSA2048.FFLEN)
	priv := RSA2048.New_private_key(RSA2048.HFLEN)

	var PT [RSA2048.RFS]byte
	var M [RSA2048.RFS]byte
	var CT [RSA2048.RFS]byte

	fmt.Printf("Generating 2048-bit RSA public/private key pair\n")

	start := time.Now()
	iterations := 0
	elapsed := time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		RSA2048.RSA_KEY_PAIR(rng, 65537, priv, pub)
		iterations++
		elapsed = time.Since(start)
	}
	dur := float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("RSA gen - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	for i := 0; i < RSA2048.RFS; i++ {
		M[i] = byte(i % 128)
	}

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		RSA2048.RSA_ENCRYPT(pub, M[:], CT[:])
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("RSA enc - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	start = time.Now()
	iterations = 0
	elapsed = time.Since(start)
	for (int(elapsed/time.Second)) < MIN_TIME || iterations < MIN_ITERS {
		RSA2048.RSA_DECRYPT(priv, CT[:], PT[:])
		iterations++
		elapsed = time.Since(start)
	}
	dur = float64(elapsed/time.Millisecond) / float64(iterations)
	fmt.Printf("RSA dec - %8d iterations  ", iterations)
	fmt.Printf(" %8.2f ms per iteration\n", dur)

	for i := 0; i < RSA2048.RFS; i++ {
		if PT[i] != M[i] {
			fmt.Printf("FAILURE - RSA decryption\n")
			return
		}
	}

	fmt.Printf("All tests pass\n")
}

func main() {
	var RAW [100]byte

	rng := core.NewRAND()

	rng.Clean()
	for i := 0; i < 100; i++ {
		RAW[i] = byte(i)
	}

	rng.Seed(100, RAW[:])

	ED_25519(rng)
	NIST_256(rng)
	GOLDI_LOCKS(rng)
	BN_254(rng)
	BLS_383(rng)
	BLS_24(rng)
	BLS_48(rng)
	RSA_2048(rng)

}
