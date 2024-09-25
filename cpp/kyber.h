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

/* Kyber API */

#ifndef KYBER_H
#define KYBER_H

#include "core.h"

#define ML_KEM    // define this for FIPS 203 standard

//q= 3329
#define KY_LGN 8
#define KY_DEGREE (1 << KY_LGN)
#define KY_PRIME 0xD01

#define KY_ONE 0x549		// R mod Q
#define KY_QINV 62209   // q^(-1) mod 2^16

#define KYBER_SECRET_CPA_SIZE_512 (2*(KY_DEGREE*3)/2)
#define KYBER_PUBLIC_SIZE_512 (32+2*(KY_DEGREE*3)/2)
#define KYBER_CIPHERTEXT_SIZE_512  ((10*2+4)*KY_DEGREE/8)
#define KYBER_SECRET_CCA_SIZE_512 (KYBER_SECRET_CPA_SIZE_512+KYBER_PUBLIC_SIZE_512+64)
#define KYBER_SHARED_SECRET_512 32

#define KYBER_SECRET_CPA_SIZE_768 (3*(KY_DEGREE*3)/2)
#define KYBER_PUBLIC_SIZE_768 (32+3*(KY_DEGREE*3)/2)
#define KYBER_CIPHERTEXT_SIZE_768  ((10*3+4)*KY_DEGREE/8)
#define KYBER_SECRET_CCA_SIZE_768 (KYBER_SECRET_CPA_SIZE_768+KYBER_PUBLIC_SIZE_768+64)
#define KYBER_SHARED_SECRET_768 32

#define KYBER_SECRET_CPA_SIZE_1024 (4*(KY_DEGREE*3)/2)
#define KYBER_PUBLIC_SIZE_1024 (32+4*(KY_DEGREE*3)/2)
#define KYBER_CIPHERTEXT_SIZE_1024  ((11*4+5)*KY_DEGREE/8)
#define KYBER_SECRET_CCA_SIZE_1024 (KYBER_SECRET_CPA_SIZE_1024+KYBER_PUBLIC_SIZE_1024+64)
#define KYBER_SHARED_SECRET_1024 32

#define KY_MAXK 4

namespace core {

/** @brief Kyber KEM CCA key pair generation
 *
    @param r64 64 random bytes
    @param SK secret key
    @param PK public key

 */
extern void KYBER512_keypair(byte *r64,octet *SK,octet *PK);

/** @brief Kyber KEM CCA encrypt
 *
    @param r32 32 random bytes
    @param PK public key
	@param SS random session key generated
	@param CT ciphertext

 */
extern void KYBER512_encrypt(byte *r32,octet *PK,octet *SS,octet *CT);

/** @brief Kyber KEM CCA decrypt
 *
    @param SK secret key
	@param CT ciphertext
	@param SS output session key 
 */
extern void KYBER512_decrypt(octet *SK,octet *CT,octet *SS);

/** @brief Kyber KEM CCA key pair generation
 *
    @param r64 64 random bytes
    @param SK secret key
    @param PK public key

 */
extern void KYBER768_keypair(byte *r64,octet *SK,octet *PK);

/** @brief Kyber KEM CCA encrypt
 *
    @param r32 32 random bytes
    @param PK public key
	@param SS random session key generated
	@param CT ciphertext

 */
extern void KYBER768_encrypt(byte *r32,octet *PK,octet *SS,octet *CT);

/** @brief Kyber KEM CCA decrypt
 *
    @param SK secret key
	@param CT ciphertext
	@param SS output session key 
 */
extern void KYBER768_decrypt(octet *SK,octet *CT,octet *SS);

/** @brief Kyber KEM CCA key pair generation
 *
    @param r64 64 random bytes
    @param SK secret key
    @param PK public key

 */
extern void KYBER1024_keypair(byte *r64,octet *SK,octet *PK);

/** @brief Kyber KEM CCA encrypt
 *
    @param r32 32 random bytes
    @param PK public key
	@param SS random session key generated
	@param CT ciphertext

 */
extern void KYBER1024_encrypt(byte *r32,octet *PK,octet *SS,octet *CT);

/** @brief Kyber KEM CCA decrypt
 *
    @param SK secret key
	@param CT ciphertext
	@param SS output session key 
 */
extern void KYBER1024_decrypt(octet *SK,octet *CT,octet *SS);
}

#endif
