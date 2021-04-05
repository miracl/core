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

/* RSA API high-level functions  */

package XXX

//import "fmt"
import "github.com/miracl/core/go/core"

const RFS int = int(MODBYTES) * FFLEN

const RSA_HASH_TYPE int = core.SHA256

type rsa_private_key struct {
	p, q, dp, dq, c *FF
}

func New_private_key(n int) *rsa_private_key {
	SK := new(rsa_private_key)
	SK.p = NewFFint(n)
	SK.q = NewFFint(n)
	SK.dp = NewFFint(n)
	SK.dq = NewFFint(n)
	SK.c = NewFFint(n)
	return SK
}

type rsa_public_key struct {
	e int
	n *FF
}

func New_public_key(m int) *rsa_public_key {
	PK := new(rsa_public_key)
	PK.e = 0
	PK.n = NewFFint(m)
	return PK
}

// Input private key from OpenSSL format
// e.g as in openssl rsa -in privkey.pem -noout -text
// Note order swap - For MIRACL c=1/p mod q, for OpenSSL c=1/q mod p
func RSA_PRIVATE_KEY_FROM_OPENSSL(P []byte, Q []byte,DP []byte, DQ []byte, C []byte, PRIV *rsa_private_key) {
    ff_fromBytes(PRIV.p,Q)
    ff_fromBytes(PRIV.q,P)
    ff_fromBytes(PRIV.dp,DQ)
    ff_fromBytes(PRIV.dq,DP)
    ff_fromBytes(PRIV.c,C)
}

func RSA_KEY_PAIR_FROM_OPENSSL(e int,P []byte, Q []byte,DP []byte, DQ []byte, C []byte, PRIV *rsa_private_key, PUB *rsa_public_key) {
	RSA_PRIVATE_KEY_FROM_OPENSSL(P,Q,DP,DQ,C,PRIV)
	PUB.n = ff_mul(PRIV.p, PRIV.q)
	PUB.e = e
}

func RSA_KEY_PAIR(rng *core.RAND, e int, PRIV *rsa_private_key, PUB *rsa_public_key) { /* IEEE1363 A16.11/A16.12 more or less */
	n := PUB.n.getlen() / 2
	t := NewFFint(n)
	p1 := NewFFint(n)
	q1 := NewFFint(n)

	for true {
		PRIV.p.random(rng)
		for PRIV.p.lastbits(2) != 3 {
			PRIV.p.inc(1)
		}
		for !prime(PRIV.p, rng) {
			PRIV.p.inc(4)
		}

		p1.copy(PRIV.p)
		p1.dec(1)

		if p1.cfactor(e) {
			continue
		}
		break
	}

	for true {
		PRIV.q.random(rng)
		for PRIV.q.lastbits(2) != 3 {
			PRIV.q.inc(1)
		}
		for !prime(PRIV.q, rng) {
			PRIV.q.inc(4)
		}

		q1.copy(PRIV.q)
		q1.dec(1)

		if q1.cfactor(e) {
			continue
		}

		break
	}

	PUB.n = ff_mul(PRIV.p, PRIV.q)
	PUB.e = e

	t.copy(p1)
	t.shr()
	PRIV.dp.set(e)
	PRIV.dp.invmodp(t)
	if PRIV.dp.parity() == 0 {
		PRIV.dp.add(t)
	}
	PRIV.dp.norm()

	t.copy(q1)
	t.shr()
	PRIV.dq.set(e)
	PRIV.dq.invmodp(t)
	if PRIV.dq.parity() == 0 {
		PRIV.dq.add(t)
	}
	PRIV.dq.norm()

	PRIV.c.copy(PRIV.p)
	PRIV.c.invmodp(PRIV.q)

}


/* RSA encryption with the public key */
func RSA_ENCRYPT(PUB *rsa_public_key, F []byte, G []byte) {
	n := PUB.n.getlen()
	f := NewFFint(n)

	ff_fromBytes(f, F)
	f.power(PUB.e, PUB.n)
	f.toBytes(G)
}

/* RSA decryption with the private key */
func RSA_DECRYPT(PRIV *rsa_private_key, G []byte, F []byte) {
	n := PRIV.p.getlen()
	g := NewFFint(2 * n)

	ff_fromBytes(g, G)
	jp := g.dmod(PRIV.p)
	jq := g.dmod(PRIV.q)

	jp.skpow(PRIV.dp, PRIV.p)
	jq.skpow(PRIV.dq, PRIV.q)

	g.zero()
	g.dscopy(jp)
	jp.mod(PRIV.q)
	if ff_comp(jp, jq) > 0 {
		jq.add(PRIV.q)
	}
	jq.sub(jp)
	jq.norm()

	t := ff_mul(PRIV.c, jq)
	jq = t.dmod(PRIV.q)

	t = ff_mul(jq, PRIV.p)
	g.add(t)
	g.norm()

	g.toBytes(F)
}

/* destroy the Private Key structure */
func RSA_PRIVATE_KEY_KILL(PRIV *rsa_private_key) {
	PRIV.p.zero()
	PRIV.q.zero()
	PRIV.dp.zero()
	PRIV.dq.zero()
	PRIV.c.zero()
}