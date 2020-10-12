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

//
//  TestALL.swift
//
//  Created by Michael Scott on 02/07/2015.
//  Copyright (c) 2015 Michael Scott. All rights reserved.
//

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */

import Foundation
import core // comment out for Xcode
import ed25519
import nist256
import goldilocks
import rsa2048

public func printBinary(_ array: [UInt8])
{
    for i in 0 ..< array.count
    {
        let h=String(format:"%02x",array[i])
        print("\(h)", terminator: "")
    }
    print(" ");
}

public func TestRSA_2048(_ rng: inout RAND)
{
    let RFS=RSA.RFS

    var message="Hello World\n"

    var pub=rsa_public_key(Int(CONFIG_FF.FFLEN))
    var priv=rsa_private_key(Int(CONFIG_FF.HFLEN))

    var ML=[UInt8](repeating: 0,count: RFS)
    var C=[UInt8](repeating: 0,count: RFS)
    var S=[UInt8](repeating: 0,count: RFS)


    print("\nGenerating RSA public/private key pair")
    RSA.KEY_PAIR(&rng,65537,&priv,&pub)

    let M=[UInt8](message.utf8)
    print("Encrypting test string\n");
    let E=HMAC.OAEP_ENCODE(RSA.HASH_TYPE,M,&rng,nil,RFS); /* OAEP encode message m to e  */

    RSA.ENCRYPT(pub,E,&C);     /* encrypt encoded message */
    print("Ciphertext= 0x", terminator: ""); printBinary(C)

    print("Decrypting test string\n");
    RSA.DECRYPT(priv,C,&ML)
    let MS=HMAC.OAEP_DECODE(RSA.HASH_TYPE,nil,&ML,RFS) /* OAEP encode message m to e  */

    message=""
    for i in 0 ..< MS.count
    {
        message+=String(UnicodeScalar(MS[i]))
    }
    print(message);

    let T=HMAC.PSS_ENCODE(RSA.HASH_TYPE,M,&rng,RFS)
    print("T= 0x",terminator: ""); printBinary(T)
    if HMAC.PSS_VERIFY(RSA.HASH_TYPE,M,T) {
        print("PSS Encoding OK\n");
    } else {
        print("PSS Encoding FAILED\n");
    }

    print("Signing message")
    HMAC.PKCS15(RSA.HASH_TYPE,M,&C,RFS)

    RSA.DECRYPT(priv,C,&S); //  create signature in S
    print("Signature= 0x",terminator: ""); printBinary(S)

    RSA.ENCRYPT(pub,S,&ML);

    var cmp=true
    if C.count != ML.count {cmp=false}
    else
    {
        for j in 0 ..< C.count
        {
            if C[j] != ML[j] {cmp=false}
        }
    }

    if cmp {print("\nSignature is valid\n")}
    else {print("\nSignature is INVALID\n")}


    RSA.PRIVATE_KEY_KILL(priv);
}


public func TestECDH_ed25519(_ rng: inout RAND)
{
    let pp=String("M0ng00se");

    let EGS=ed25519.ECDH.EGS
    let EFS=ed25519.ECDH.EFS
    let EAS=ed25519.CONFIG_CURVE.AESKEY
    let sha=ed25519.CONFIG_CURVE.HASH_TYPE

    var S1=[UInt8](repeating: 0,count: EGS)
    var W0=[UInt8](repeating: 0,count: 2*EFS+1)
    var W1=[UInt8](repeating: 0,count: 2*EFS+1)
    var Z0=[UInt8](repeating: 0,count: EFS)
    var Z1=[UInt8](repeating: 0,count: EFS)

    var SALT=[UInt8](repeating: 0,count: 8)
    var P1=[UInt8](repeating: 0,count: 3)
    var P2=[UInt8](repeating: 0,count: 4)
    var V=[UInt8](repeating: 0,count: 2*EFS+1)
    var M=[UInt8](repeating: 0,count: 17)
    var T=[UInt8](repeating: 0,count: 12)
    var CS=[UInt8](repeating: 0,count: EGS)
    var DS=[UInt8](repeating: 0,count: EGS)

    var NULLRNG : RAND? = nil
    var REALRNG : RAND? = rng

    for i in 0 ..< 8 {SALT[i]=UInt8(i+1)}  // set Salt

    print("\nTest Curve ed25519");
    print("Alice's Passphrase= " + pp)
    let PW=[UInt8]( (pp).utf8 )

    /* private key S0 of size EGS bytes derived from Password and Salt */

/* Note use of curve name here to disambiguate between supported curves!! */
/* Not needed if only one curve supported */

    var S0=HMAC.PBKDF2(HMAC.MC_SHA2,sha,PW,SALT,1000,EGS)
    print("Alice's private key= 0x",terminator: ""); printBinary(S0)



    /* Generate Key pair S/W */
    ed25519.ECDH.KEY_PAIR_GENERATE(&NULLRNG,&S0,&W0);

    print("Alice's public key= 0x",terminator: ""); printBinary(W0)

    var res=ed25519.ECDH.PUBLIC_KEY_VALIDATE(W0);

    if res != 0
    {
        print("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    ed25519.ECDH.KEY_PAIR_GENERATE(&REALRNG,&S1,&W1)

    print("Servers private key= 0x",terminator: ""); printBinary(S1)

    print("Servers public key= 0x",terminator: ""); printBinary(W1);

    res=ed25519.ECDH.PUBLIC_KEY_VALIDATE(W1)
    if res != 0
    {
        print("ECP Public Key is invalid!")
        return
    }

    /* Calculate common key using DH - IEEE 1363 method */

    ed25519.ECDH.ECPSVDP_DH(S0,W1,&Z0,0)
    ed25519.ECDH.ECPSVDP_DH(S1,W0,&Z1,0)

    var same=true
    for i in 0 ..< EFS
    {
        if Z0[i] != Z1[i] {same=false}
    }

    if (!same)
    {
        print("*** ECPSVDP-DH Failed")
        return
    }

    let KEY=HMAC.KDF2(HMAC.MC_SHA2,sha,Z0,nil,EAS)

    print("Alice's DH Key=  0x",terminator: ""); printBinary(KEY)
    print("Servers DH Key=  0x",terminator: ""); printBinary(KEY)

    if ed25519.CONFIG_CURVE.CURVETYPE != ed25519.CONFIG_CURVE.MONTGOMERY
    {
        print("Testing ECIES")

        P1[0]=0x0; P1[1]=0x1; P1[2]=0x2
        P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3

        for i in 0...16 {M[i]=UInt8(i&0xff)}

        let C=ed25519.ECDH.ECIES_ENCRYPT(sha,P1,P2,&REALRNG,W1,M,&V,&T)

        print("Ciphertext= ")
        print("V= 0x",terminator: ""); printBinary(V)
        print("C= 0x",terminator: ""); printBinary(C)
        print("T= 0x",terminator: ""); printBinary(T)

        M=ed25519.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1)
        if M.count==0
        {
            print("*** ECIES Decryption Failed\n")
            return
        }
        else {print("Decryption succeeded")}

        print("Message is 0x",terminator: ""); printBinary(M)

        print("Testing ECDSA")

        if ed25519.ECDH.ECPSP_DSA(sha,&rng,S0,M,&CS,&DS) != 0
        {
            print("***ECDSA Signature Failed")
            return
        }
        print("Signature= ")
        print("C= 0x",terminator: ""); printBinary(CS)
        print("D= 0x",terminator: ""); printBinary(DS)

        if ed25519.ECDH.ECPVP_DSA(sha,W0,M,CS,DS) != 0
        {
            print("***ECDSA Verification Failed")
            return
        }
        else {print("ECDSA Signature/Verification succeeded ")}
    }
    rng=REALRNG!
}


public func TestECDH_nist256(_ rng: inout RAND)
{
    let pp=String("M0ng00se");

    let EGS=nist256.ECDH.EGS
    let EFS=nist256.ECDH.EFS
    let EAS=nist256.CONFIG_CURVE.AESKEY
    let sha=nist256.CONFIG_CURVE.HASH_TYPE

    var S1=[UInt8](repeating: 0,count: EGS)
    var W0=[UInt8](repeating: 0,count: 2*EFS+1)
    var W1=[UInt8](repeating: 0,count: 2*EFS+1)
    var Z0=[UInt8](repeating: 0,count: EFS)
    var Z1=[UInt8](repeating: 0,count: EFS)

    var SALT=[UInt8](repeating: 0,count: 8)
    var P1=[UInt8](repeating: 0,count: 3)
    var P2=[UInt8](repeating: 0,count: 4)
    var V=[UInt8](repeating: 0,count: 2*EFS+1)
    var M=[UInt8](repeating: 0,count: 17)
    var T=[UInt8](repeating: 0,count: 12)
    var CS=[UInt8](repeating: 0,count: EGS)
    var DS=[UInt8](repeating: 0,count: EGS)

    var NULLRNG : RAND? = nil
    var REALRNG : RAND? = rng

    for i in 0 ..< 8 {SALT[i]=UInt8(i+1)}  // set Salt

    print("\nTest Curve nist256");
    print("Alice's Passphrase= " + pp)
    let PW=[UInt8]( (pp).utf8 )

    /* private key S0 of size EGS bytes derived from Password and Salt */

/* Note use of curve name here to disambiguate between supported curves!! */
/* Not needed if only one curve supported */

    var S0=HMAC.PBKDF2(HMAC.MC_SHA2,sha,PW,SALT,1000,EGS)
    print("Alice's private key= 0x",terminator: ""); printBinary(S0)

    /* Generate Key pair S/W */
    nist256.ECDH.KEY_PAIR_GENERATE(&NULLRNG,&S0,&W0)

    print("Alice's public key= 0x",terminator: ""); printBinary(W0)

    var res=nist256.ECDH.PUBLIC_KEY_VALIDATE(W0)

    if res != 0
    {
        print("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    nist256.ECDH.KEY_PAIR_GENERATE(&REALRNG,&S1,&W1)

    print("Servers private key= 0x",terminator: ""); printBinary(S1)

    print("Servers public key= 0x",terminator: ""); printBinary(W1)

    res=nist256.ECDH.PUBLIC_KEY_VALIDATE(W1)
    if res != 0
    {
        print("ECP Public Key is invalid!")
        return
    }

    /* Calculate common key using DH - IEEE 1363 method */

    nist256.ECDH.ECPSVDP_DH(S0,W1,&Z0,0)
    nist256.ECDH.ECPSVDP_DH(S1,W0,&Z1,0)

    var same=true
    for i in 0 ..< EFS
    {
        if Z0[i] != Z1[i] {same=false}
    }

    if (!same)
    {
        print("*** ECPSVDP-DH Failed")
        return
    }

    let KEY=HMAC.KDF2(HMAC.MC_SHA2,sha,Z0,nil,EAS)

    print("Alice's DH Key=  0x",terminator: ""); printBinary(KEY)
    print("Servers DH Key=  0x",terminator: ""); printBinary(KEY)

    if nist256.CONFIG_CURVE.CURVETYPE != nist256.CONFIG_CURVE.MONTGOMERY
    {
        print("Testing ECIES")

        P1[0]=0x0; P1[1]=0x1; P1[2]=0x2
        P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3

        for i in 0...16 {M[i]=UInt8(i&0xff)}

        let C=nist256.ECDH.ECIES_ENCRYPT(sha,P1,P2,&REALRNG,W1,M,&V,&T)

        print("Ciphertext= ")
        print("V= 0x",terminator: ""); printBinary(V)
        print("C= 0x",terminator: ""); printBinary(C)
        print("T= 0x",terminator: ""); printBinary(T)

        M=nist256.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1)
        if M.count==0
        {
            print("*** ECIES Decryption Failed\n")
            return
        }
        else {print("Decryption succeeded")}

        print("Message is 0x",terminator: ""); printBinary(M)

        print("Testing ECDSA")

        if nist256.ECDH.ECPSP_DSA(sha,&rng,S0,M,&CS,&DS) != 0
        {
            print("***ECDSA Signature Failed")
            return
        }
        print("Signature= ")
        print("C= 0x",terminator: ""); printBinary(CS)
        print("D= 0x",terminator: ""); printBinary(DS)

        if nist256.ECDH.ECPVP_DSA(sha,W0,M,CS,DS) != 0
        {
            print("***ECDSA Verification Failed")
            return
        }
        else {print("ECDSA Signature/Verification succeeded ")}
    }
    rng=REALRNG!
}

public func TestECDH_goldilocks(_ rng: inout RAND)
{
    let pp=String("M0ng00se");

    let EGS=goldilocks.ECDH.EGS
    let EFS=goldilocks.ECDH.EFS
    let EAS=goldilocks.CONFIG_CURVE.AESKEY
    let sha=goldilocks.CONFIG_CURVE.HASH_TYPE

    var S1=[UInt8](repeating: 0,count: EGS)
    var W0=[UInt8](repeating: 0,count: 2*EFS+1)
    var W1=[UInt8](repeating: 0,count: 2*EFS+1)
    var Z0=[UInt8](repeating: 0,count: EFS)
    var Z1=[UInt8](repeating: 0,count: EFS)

    var SALT=[UInt8](repeating: 0,count: 8)
    var P1=[UInt8](repeating: 0,count: 3)
    var P2=[UInt8](repeating: 0,count: 4)
    var V=[UInt8](repeating: 0,count: 2*EFS+1)
    var M=[UInt8](repeating: 0,count: 17)
    var T=[UInt8](repeating: 0,count: 12)
    var CS=[UInt8](repeating: 0,count: EGS)
    var DS=[UInt8](repeating: 0,count: EGS)

    var NULLRNG : RAND? = nil
    var REALRNG : RAND? = rng

    for i in 0 ..< 8 {SALT[i]=UInt8(i+1)}  // set Salt

    print("\nTest Curve goldilocks");
    print("Alice's Passphrase= " + pp)
    let PW=[UInt8]( (pp).utf8 )

    /* private key S0 of size EGS bytes derived from Password and Salt */

/* Note use of curve name here to disambiguate between supported curves!! */
/* Not needed if only one curve supported */

    var S0=HMAC.PBKDF2(HMAC.MC_SHA2,sha,PW,SALT,1000,EGS)
    print("Alice's private key= 0x",terminator: ""); printBinary(S0)

    /* Generate Key pair S/W */
    goldilocks.ECDH.KEY_PAIR_GENERATE(&NULLRNG,&S0,&W0);

    print("Alice's public key= 0x",terminator: ""); printBinary(W0)

    var res=goldilocks.ECDH.PUBLIC_KEY_VALIDATE(W0);

    if res != 0
    {
        print("ECP Public Key is invalid!");
        return;
    }

    /* Random private key for other party */
    goldilocks.ECDH.KEY_PAIR_GENERATE(&REALRNG,&S1,&W1)

    print("Servers private key= 0x",terminator: ""); printBinary(S1)

    print("Servers public key= 0x",terminator: ""); printBinary(W1);

    res=goldilocks.ECDH.PUBLIC_KEY_VALIDATE(W1)
    if res != 0
    {
        print("ECP Public Key is invalid!")
        return
    }

    /* Calculate common key using DH - IEEE 1363 method */

    goldilocks.ECDH.ECPSVDP_DH(S0,W1,&Z0,0)
    goldilocks.ECDH.ECPSVDP_DH(S1,W0,&Z1,0)

    var same=true
    for i in 0 ..< EFS
    {
        if Z0[i] != Z1[i] {same=false}
    }

    if (!same)
    {
        print("*** ECPSVDP-DH Failed")
        return
    }

    let KEY=HMAC.KDF2(HMAC.MC_SHA2,sha,Z0,nil,EAS)

    print("Alice's DH Key=  0x",terminator: ""); printBinary(KEY)
    print("Servers DH Key=  0x",terminator: ""); printBinary(KEY)

    if goldilocks.CONFIG_CURVE.CURVETYPE != goldilocks.CONFIG_CURVE.MONTGOMERY
    {
        print("Testing ECIES")

        P1[0]=0x0; P1[1]=0x1; P1[2]=0x2
        P2[0]=0x0; P2[1]=0x1; P2[2]=0x2; P2[3]=0x3

        for i in 0...16 {M[i]=UInt8(i&0xff)}

        let C=goldilocks.ECDH.ECIES_ENCRYPT(sha,P1,P2,&REALRNG,W1,M,&V,&T)

        print("Ciphertext= ")
        print("V= 0x",terminator: ""); printBinary(V)
        print("C= 0x",terminator: ""); printBinary(C)
        print("T= 0x",terminator: ""); printBinary(T)

        M=goldilocks.ECDH.ECIES_DECRYPT(sha,P1,P2,V,C,T,S1)
        if M.count==0
        {
            print("*** ECIES Decryption Failed\n")
            return
        }
        else {print("Decryption succeeded")}

        print("Message is 0x",terminator: ""); printBinary(M)

        print("Testing ECDSA")

        if goldilocks.ECDH.ECPSP_DSA(sha,&rng,S0,M,&CS,&DS) != 0
        {
            print("***ECDSA Signature Failed")
            return
        }
        print("Signature= ")
        print("C= 0x",terminator: ""); printBinary(CS)
        print("D= 0x",terminator: ""); printBinary(DS)

        if goldilocks.ECDH.ECPVP_DSA(sha,W0,M,CS,DS) != 0
        {
            print("***ECDSA Verification Failed")
            return
        }
        else {print("ECDSA Signature/Verification succeeded ")}
    }
    rng=REALRNG!
}

var RAW=[UInt8](repeating: 0,count: 100)
var rng=RAND()

rng.clean();
for i in 0 ..< 100 {RAW[i]=UInt8(i&0xff)}
rng.seed(100,RAW)


TestECDH_ed25519(&rng)
TestECDH_nist256(&rng)
TestECDH_goldilocks(&rng)
TestRSA_2048(&rng)

