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

/* Kyber768 parameters */

//q= 3329
//#define KY_WL 32
#define KY_LGN 8
#define KY_DEGREE (1 << KY_LGN)
#define KY_PRIME 0xD01

// These vary for different security levels

// Kyber 512
//#define KY_K 2
//#define KY_ETA1 3
//#define KY_DU 10
//#define KY_DV 4

// Kyber 768
#define KY_K 3
#define KY_ETA1 2
#define KY_DU 10
#define KY_DV 4

// Kyber 1024
//#define KY_K 4
//#define KY_ETA1 2
//#define KY_DU 11
//#define KY_DV 5

#define KY_ETA2 2
#define KY_ONE 0x549		// R mod Q
//#define KY_COMBO 0x450		// ONE*inv mod Q
//#define KY_R2MODP 0xBAC	// R^2 mod Q
//#define KY_ND 0x94570CFF   // 1/(R-Q) mod R

//#define KY_MONT 2285 // 2^16 % Q
#define KY_QINV 62209 // q^(-1) mod 2^16

#define KYBER_SECRET_CPA (KY_K*(KY_DEGREE*3)/2)
#define KYBER_PUBLIC (32+KY_K*(KY_DEGREE*3)/2)
#define KYBER_CIPHERTEXT ((KY_DU*KY_K+KY_DV)*KY_DEGREE/8)
#define KYBER_SECRET_CCA (KYBER_SECRET_CPA+KYBER_PUBLIC+64)

/** @brief Kyber KEM CPA key pair generation
 *
    @param tau random bytes
    @param SK secret key
    @param PK public key

 */
extern void KYBER_CPA_keypair(byte *tau,octet *SK,octet *PK);

/** @brief Kyber KEM CPA encrypt
 *
    @param coins random bytes
    @param PK public key
	@param SS session key to be encrypted
	@param CT ciphertext

 */
extern void KYBER_CPA_encrypt(byte *coins,octet *PK,octet *SS,octet *CT);

/** @brief Kyber KEM CPA decrypt
 *
    @param SK secret key
	@param CT ciphertext
	@param SS output session key
 */
extern void KYBER_CPA_decrypt(octet *PK,octet *CT,octet *SS);


/** @brief Kyber KEM CCA key pair generation
 *
    @param r64 64 random bytes
    @param SK secret key
    @param PK public key

 */
extern void KYBER_CCA_keypair(byte *r64,octet *SK,octet *PK);

/** @brief Kyber KEM CCA encrypt
 *
    @param r32 32 random bytes
    @param PK public key
	@param SS random session key generated
	@param CT ciphertext

 */
extern void KYBER_CCA_encrypt(byte *r32,octet *PK,octet *SS,octet *CT);

/** @brief Kyber KEM CCA decrypt
 *
    @param SK secret key
	@param CT ciphertext
	@param SS output session key 
 */
extern void KYBER_CCA_decrypt(octet *PK,octet *CT,octet *SS);

#endif
