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

//
//  TestECDH.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

import Foundation
import core // comment out for Xcode
import ed25519
import nist256
import goldilocks
import bn254
import bls12383
import bls24479
import bls48556
import rsa2048

public func TimeRSA_2048(_ rng: inout RAND)
{
    let RFS=RSA.RFS
    let MIN_TIME=10.0
    let MIN_ITERS=10
    var fail=false;

    var pub=rsa_public_key(Int(CONFIG_FF.FFLEN))
    var priv=rsa_private_key(Int(CONFIG_FF.HFLEN))

    var M=[UInt8](repeating: 0,count: RFS)
    var C=[UInt8](repeating: 0,count: RFS)
    var P=[UInt8](repeating: 0,count: RFS)

    print("\nTiming/Testing 2048-bit RSA")
    print("Generating public/private key pair")

    var start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        RSA.KEY_PAIR(&rng,65537,&priv,&pub)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "RSA gen - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    for i in 0..<RFS {M[i]=UInt8(i%128)}

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        RSA.ENCRYPT(pub,M,&C)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "RSA enc - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        RSA.DECRYPT(priv,C,&P)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "RSA dec - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var cmp=true
    for i in 0..<RFS {
        if P[i] != M[i] {cmp=false}
    }

    if !cmp {
        print("FAILURE - RSA decryption")
        fail=true;
    }

    if !fail {
       print("All tests pass")
    }
}


public func TimeECDH_ed25519(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10

    var fail=false;

    print("\nTiming/Testing ED25519 ECC")
    if ed25519.CONFIG_CURVE.CURVETYPE==ed25519.CONFIG_CURVE.WEIERSTRASS {
        print("Weierstrass parameterisation")
    }
    if ed25519.CONFIG_CURVE.CURVETYPE==ed25519.CONFIG_CURVE.EDWARDS {
        print("Edwards parameterisation")
    }
    if ed25519.CONFIG_CURVE.CURVETYPE==ed25519.CONFIG_CURVE.MONTGOMERY {
        print("Montgomery representation")
    }
    if ed25519.CONFIG_FIELD.MODTYPE==ed25519.CONFIG_FIELD.PSEUDO_MERSENNE {
        print("Pseudo-Mersenne Modulus")
    }
    if ed25519.CONFIG_FIELD.MODTYPE==ed25519.CONFIG_FIELD.MONTGOMERY_FRIENDLY {
        print("Montgomery Friendly Modulus")
    }
    if ed25519.CONFIG_FIELD.MODTYPE==ed25519.CONFIG_FIELD.GENERALISED_MERSENNE {
        print("Generalised-Mersenne Modulus")
    }
    if ed25519.CONFIG_FIELD.MODTYPE==ed25519.CONFIG_FIELD.NOT_SPECIAL {
        print("Not special Modulus")
    }
    print("Modulus size \(ed25519.CONFIG_FIELD.MODBITS) bits")
    print("\(ed25519.CONFIG_BIG.CHUNK) bit build")


    var s:ed25519.BIG
    var W:ed25519.ECP
    let G=ed25519.ECP.generator();

    let r=ed25519.BIG(ed25519.ROM.CURVE_Order)
    s=ed25519.BIG.randtrunc(r,16*ed25519.CONFIG_CURVE.AESKEY,&rng)
    for _ in 0..<10 {
        s=ed25519.BIG.randtrunc(r,16*ed25519.CONFIG_CURVE.AESKEY,&rng)
        W=ed25519.ECP.hashit(s);
        if W.is_infinity() {
            print("HASHING FAILURE - P=O")
            fail=true;
        }
    }

    W=G.mul(r)
    if !W.is_infinity() {
        print("FAILURE - rG!=O")
        fail=true;
    }

    let start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=G.mul(s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "EC  mul - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    if !fail {
       print("All tests pass")
    }
}

public func TimeECDH_nist256(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10

    var fail=false;

    print("\nTiming/Testing nist256 ECC")
    if nist256.CONFIG_CURVE.CURVETYPE==nist256.CONFIG_CURVE.WEIERSTRASS {
        print("Weierstrass parameterisation")
    }
    if nist256.CONFIG_CURVE.CURVETYPE==nist256.CONFIG_CURVE.EDWARDS {
        print("Edwards parameterisation")
    }
    if nist256.CONFIG_CURVE.CURVETYPE==nist256.CONFIG_CURVE.MONTGOMERY {
        print("Montgomery representation")
    }
    if nist256.CONFIG_FIELD.MODTYPE==nist256.CONFIG_FIELD.PSEUDO_MERSENNE {
        print("Pseudo-Mersenne Modulus")
    }
    if nist256.CONFIG_FIELD.MODTYPE==nist256.CONFIG_FIELD.MONTGOMERY_FRIENDLY {
        print("Montgomery Friendly Modulus")
    }
    if nist256.CONFIG_FIELD.MODTYPE==nist256.CONFIG_FIELD.GENERALISED_MERSENNE {
        print("Generalised-Mersenne Modulus")
    }
    if nist256.CONFIG_FIELD.MODTYPE==nist256.CONFIG_FIELD.NOT_SPECIAL {
        print("Not special Modulus")
    }
    print("Modulus size \(nist256.CONFIG_FIELD.MODBITS) bits")
    print("\(nist256.CONFIG_BIG.CHUNK) bit build")


    var s:nist256.BIG
    let G=nist256.ECP.generator();

    let r=nist256.BIG(nist256.ROM.CURVE_Order)
    s=nist256.BIG.randtrunc(r,16*nist256.CONFIG_CURVE.AESKEY,&rng)

    var W=nist256.ECP.hashit(s);
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    W=G.mul(r)
    if !W.is_infinity() {
        print("FAILURE - rG!=O")
        fail=true;
    }

    let start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=G.mul(s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "EC  mul - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    if !fail {
       print("All tests pass")
    }
}

public func TimeECDH_goldilocks(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10

    var fail=false;

    print("\nTiming/Testing goldilocks ECC")
    if goldilocks.CONFIG_CURVE.CURVETYPE==goldilocks.CONFIG_CURVE.WEIERSTRASS {
        print("Weierstrass parameterisation")
    }
    if goldilocks.CONFIG_CURVE.CURVETYPE==goldilocks.CONFIG_CURVE.EDWARDS {
        print("Edwards parameterisation")
    }
    if goldilocks.CONFIG_CURVE.CURVETYPE==goldilocks.CONFIG_CURVE.MONTGOMERY {
        print("Montgomery representation")
    }
    if goldilocks.CONFIG_FIELD.MODTYPE==goldilocks.CONFIG_FIELD.PSEUDO_MERSENNE {
        print("Pseudo-Mersenne Modulus")
    }
    if goldilocks.CONFIG_FIELD.MODTYPE==goldilocks.CONFIG_FIELD.MONTGOMERY_FRIENDLY {
        print("Montgomery Friendly Modulus")
    }
    if goldilocks.CONFIG_FIELD.MODTYPE==goldilocks.CONFIG_FIELD.GENERALISED_MERSENNE {
        print("Generalised-Mersenne Modulus")
    }
    if goldilocks.CONFIG_FIELD.MODTYPE==goldilocks.CONFIG_FIELD.NOT_SPECIAL {
        print("Not special Modulus")
    }
    print("Modulus size \(goldilocks.CONFIG_FIELD.MODBITS) bits")
    print("\(goldilocks.CONFIG_BIG.CHUNK) bit build")


    var s:goldilocks.BIG
    let G=goldilocks.ECP.generator();

    let r=goldilocks.BIG(goldilocks.ROM.CURVE_Order)
    s=goldilocks.BIG.randtrunc(r,16*goldilocks.CONFIG_CURVE.AESKEY,&rng)

    var W=goldilocks.ECP.hashit(s);
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    W=G.mul(r)
    if !W.is_infinity() {
        print("FAILURE - rG!=O")
        fail=true;
    }

    let start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=G.mul(s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "EC  mul - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    if !fail {
       print("All tests pass")
    }
}

public func TimeMPIN_bn254(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10
    var fail=false;

    print("\nTiming/Testing BN254 Pairings")
    if bn254.CONFIG_CURVE.CURVE_PAIRING_TYPE==bn254.CONFIG_CURVE.BN {
        print("BN Pairing-Friendly Curve")
    }
    if bn254.CONFIG_CURVE.CURVE_PAIRING_TYPE==bn254.CONFIG_CURVE.BLS {
        print("BLS Pairing-Friendly Curve")
    }
    print("Modulus size \(bn254.CONFIG_FIELD.MODBITS) bits")
    print("\(bn254.CONFIG_BIG.CHUNK) bit build")

    let G=bn254.ECP.generator();

    let r=bn254.BIG(bn254.ROM.CURVE_Order)
    let s=bn254.BIG.randtrunc(r,16*bn254.CONFIG_CURVE.AESKEY,&rng)

    var P=bn254.ECP.hashit(s);
    if P.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    P=bn254.PAIR.G1mul(G,r);

    if !P.is_infinity() {
        print("FAILURE - rP!=O")
        fail=true
    }

    var start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        P=bn254.PAIR.G1mul(G,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G1  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var Q=bn254.ECP2.generator()


    var W = bn254.ECP2.hashit(s)
    W.cfp();
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true
    }
    W = bn254.PAIR.G2mul(W, r);
    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }


    W=bn254.PAIR.G2mul(Q,r)

    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=bn254.PAIR.G2mul(Q,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G2  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var w=bn254.PAIR.ate(Q,P)
    w=bn254.PAIR.fexp(w)
    var g=bn254.PAIR.GTpow(w,r)
    if !g.isunity() {
        print("FAILURE - g^r!=1")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=bn254.PAIR.GTpow(w,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    g.copy(w)

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        _=g.compow(s,r)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow (compressed) - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        w=bn254.PAIR.ate(Q,P)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing ATE          - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=bn254.PAIR.fexp(w)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing FEXP         - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    P.copy(G)
    Q.copy(W)

    P=bn254.PAIR.G1mul(P,s)
    g=bn254.PAIR.ate(Q,P)
    g=bn254.PAIR.fexp(g)

    P.copy(G)
    Q=bn254.PAIR.G2mul(Q,s)
    w=bn254.PAIR.ate(Q,P)
    w=bn254.PAIR.fexp(w)

    if !bn254.PAIR.G1member(P) {
        print("FAILURE - P is not in G1")
        fail=true
    }

    if !bn254.PAIR.G2member(Q) {
        print("FAILURE - Q is not in G2")
        fail=true
    }

    if !bn254.PAIR.GTmember(w) {
        print("FAILURE - e(Q,P) is not in GT")
        fail=true
    }

    if !g.equals(w) {
        print("FAILURE - e(sQ,P)!=e(Q,sP)")
        fail=true
    }

    if !fail {
        print("All tests pass")
    }
}

public func TimeMPIN_bls12383(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10
    var fail=false;

    print("\nTiming/Testing bls12383 Pairings")
    if bls12383.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls12383.CONFIG_CURVE.BN {
        print("BN Pairing-Friendly Curve")
    }
    if bls12383.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls12383.CONFIG_CURVE.BLS {
        print("BLS Pairing-Friendly Curve")
    }
    print("Modulus size \(bls12383.CONFIG_FIELD.MODBITS) bits")
    print("\(bls12383.CONFIG_BIG.CHUNK) bit build")

    let G=bls12383.ECP.generator();

    let r=bls12383.BIG(bls12383.ROM.CURVE_Order)
    let s=bls12383.BIG.randtrunc(r,16*bls12383.CONFIG_CURVE.AESKEY,&rng)

    var P=bls12383.ECP.hashit(s);
    if P.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    P=bls12383.PAIR.G1mul(G,r);

    if !P.is_infinity() {
        print("FAILURE - rP!=O")
        fail=true
    }

    var start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        P=bls12383.PAIR.G1mul(G,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G1  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var Q=bls12383.ECP2.generator();

    var W = bls12383.ECP2.hashit(s)
    W.cfp();
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true
    }
    W = bls12383.PAIR.G2mul(W, r);
    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }


    W=bls12383.PAIR.G2mul(Q,r)

    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=bls12383.PAIR.G2mul(Q,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G2  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var w=bls12383.PAIR.ate(Q,P)
    w=bls12383.PAIR.fexp(w)

    var g=bls12383.PAIR.GTpow(w,r)

    if !g.isunity() {
        print("FAILURE - g^r!=1")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=bls12383.PAIR.GTpow(w,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    g.copy(w)

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        _=g.compow(s,r)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow (compressed) - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        w=bls12383.PAIR.ate(Q,P)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing ATE          - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=bls12383.PAIR.fexp(w)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing FEXP         - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    P.copy(G)
    Q.copy(W)

    P=bls12383.PAIR.G1mul(P,s)
    g=bls12383.PAIR.ate(Q,P)
    g=bls12383.PAIR.fexp(g)

    P.copy(G)
    Q=bls12383.PAIR.G2mul(Q,s)
    w=bls12383.PAIR.ate(Q,P)
    w=bls12383.PAIR.fexp(w)

    if !bls12383.PAIR.G1member(P) {
        print("FAILURE - P is not in G1")
        fail=true
    }

    if !bls12383.PAIR.G2member(Q) {
        print("FAILURE - Q is not in G2")
        fail=true
    }

    if !bls12383.PAIR.GTmember(w) {
        print("FAILURE - e(Q,P) is not in GT")
        fail=true
    }

    if !g.equals(w) {
        print("FAILURE - e(sQ,P)!=e(Q,sP)")
        fail=true
    }

    if !fail {
        print("All tests pass")
    }
}

public func TimeMPIN_bls24479(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10
    var fail=false;


    print("\nTiming/Testing bls24479 Pairings")
    if bls24479.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls24479.CONFIG_CURVE.BN {
        print("BN Pairing-Friendly Curve")
    }
    if bls24479.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls24479.CONFIG_CURVE.BLS {
        print("bls24479 Pairing-Friendly Curve")
    }
    print("Modulus size \(bls24479.CONFIG_FIELD.MODBITS) bits")
    print("\(bls24479.CONFIG_BIG.CHUNK) bit build")

    let G=bls24479.ECP.generator();

    let r=bls24479.BIG(bls24479.ROM.CURVE_Order)
    let s=bls24479.BIG.randtrunc(r,16*bls24479.CONFIG_CURVE.AESKEY,&rng)

    var P=bls24479.ECP.hashit(s);
    if P.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    P=bls24479.PAIR192.G1mul(G,r);

    if !P.is_infinity() {
        print("FAILURE - rP!=O")
        fail=true
    }

    var start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        P=bls24479.PAIR192.G1mul(G,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G1  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var Q=bls24479.ECP4.generator();

    var W = bls24479.ECP4.hashit(s)
    W.cfp();
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true
    }
    W = bls24479.PAIR192.G2mul(W, r);
    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    W=PAIR192.G2mul(Q,r)

    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=PAIR192.G2mul(Q,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G2  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var w=PAIR192.ate(Q,P)
    w=PAIR192.fexp(w)

    var g=PAIR192.GTpow(w,r)

    if !g.isunity() {
        print("FAILURE - g^r!=1")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=PAIR192.GTpow(w,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        w=PAIR192.ate(Q,P)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing ATE          - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=PAIR192.fexp(w)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing FEXP         - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    P.copy(G)
    Q.copy(W)

    P=PAIR192.G1mul(P,s)
    g=PAIR192.ate(Q,P)
    g=PAIR192.fexp(g)

    P.copy(G)
    Q=PAIR192.G2mul(Q,s)
    w=PAIR192.ate(Q,P)
    w=PAIR192.fexp(w)

    if !PAIR192.G1member(P) {
        print("FAILURE - P is not in G1")
        fail=true
    }

    if !PAIR192.G2member(Q) {
        print("FAILURE - Q is not in G2")
        fail=true
    }

    if !PAIR192.GTmember(w) {
        print("FAILURE - e(Q,P) is not in GT")
        fail=true
    }

    if !g.equals(w) {
        print("FAILURE - e(sQ,P)!=e(Q,sP)")
        fail=true
    }

    if !fail {
        print("All tests pass")
    }
}


public func TimeMPIN_bls48556(_ rng: inout RAND)
{
    let MIN_TIME=10.0
    let MIN_ITERS=10
    var fail=false;

    print("\nTiming/Testing bls48556 Pairings")
    if bls48556.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls48556.CONFIG_CURVE.BN {
        print("BN Pairing-Friendly Curve")
    }
    if bls48556.CONFIG_CURVE.CURVE_PAIRING_TYPE==bls48556.CONFIG_CURVE.BLS {
        print("bls48556 Pairing-Friendly Curve")
    }
    print("Modulus size \(bls48556.CONFIG_FIELD.MODBITS) bits")
    print("\(bls48556.CONFIG_BIG.CHUNK) bit build")

    let G=bls48556.ECP.generator();

    let r=bls48556.BIG(bls48556.ROM.CURVE_Order)
    let s=bls48556.BIG.randtrunc(r,16*bls48556.CONFIG_CURVE.AESKEY,&rng)

    var P=bls48556.ECP.hashit(s);
    if P.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true;
    }

    P=PAIR256.G1mul(G,r);

    if !P.is_infinity() {
        print("FAILURE - rP!=O")
        fail=true
    }

    var start=Date()
    var iterations=0
    var elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        P=PAIR256.G1mul(G,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G1  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var Q=ECP8.generator();

    var W = bls48556.ECP8.hashit(s)
    W.cfp();
    if W.is_infinity() {
        print("HASHING FAILURE - P=O")
        fail=true
    }
    W = bls48556.PAIR256.G2mul(W, r);
    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    W=PAIR256.G2mul(Q,r)

    if !W.is_infinity() {
        print("FAILURE - rQ!=O")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        W=PAIR256.G2mul(Q,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "G2  mul              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    var w=PAIR256.ate(Q,P)
    w=PAIR256.fexp(w)

    var g=PAIR256.GTpow(w,r)

    if !g.isunity() {
        print("FAILURE - g^r!=1")
        fail=true
    }

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=PAIR256.GTpow(w,s)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "GT  pow              - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        w=PAIR256.ate(Q,P)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing ATE          - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    start=Date()
    iterations=0
    elapsed=0.0
    while elapsed<MIN_TIME || iterations<MIN_ITERS {
        g=PAIR256.fexp(w)
        iterations+=1
        elapsed = -start.timeIntervalSinceNow
    }
    elapsed=1000.0*elapsed/Double(iterations)
    print(String(format: "PAIRing FEXP         - %d iterations",iterations),terminator: "");
    print(String(format: " %.2f ms per iteration",elapsed))

    P.copy(G)
    Q.copy(W)

    P=PAIR256.G1mul(P,s)
    g=PAIR256.ate(Q,P)
    g=PAIR256.fexp(g)

    P.copy(G)
    Q=PAIR256.G2mul(Q,s)
    w=PAIR256.ate(Q,P)
    w=PAIR256.fexp(w)

    if !PAIR256.G1member(P) {
        print("FAILURE - P is not in G1")
        fail=true
    }

    if !PAIR256.G2member(Q) {
        print("FAILURE - Q is not in G2")
        fail=true
    }

    if !PAIR256.GTmember(w) {
        print("FAILURE - e(Q,P) is not in GT")
        fail=true
    }

    if !g.equals(w) {
        print("FAILURE - e(sQ,P)!=e(Q,sP)")
        fail=true
    }

    if !fail {
        print("All tests pass")
    }
}


var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()

rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)

TimeECDH_ed25519(&rng)
TimeECDH_nist256(&rng)
TimeECDH_goldilocks(&rng)
TimeRSA_2048(&rng)
TimeMPIN_bn254(&rng)
TimeMPIN_bls12383(&rng)
TimeMPIN_bls24479(&rng)
TimeMPIN_bls48556(&rng)

