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

/* Dilithium API */

#ifndef DILITHIUM_H
#define DILITHIUM_H

#include "core.h"

/* DILITHIUM 3 parameters */

//q= 8380417
#define LGN 8
#define DEGREE (1 << LGN)
#define PRIME 0x7fe001
#define D 13
#define TD (23-D)
#define WC 49
#define LG 19
#define GAMMA1 ((sign32)1<<LG)
#define RR 32
#define GAMMA2 (PRIME-1)/RR
#define K 6
#define L 5
#define ETA 4
#define LG2ETA1 4  // lg(2*ETA+1) rounded up
#define BETA 196
#define OMEGA 55
#define YBYTES ((LG+1)*DEGREE)/8
#define W1B 4
#define ONE 0x3FFE00    // R mod Q
#define COMBO 0xA3FA    // ONE*inv mod Q
#define R2MODP 0x2419FF // R^2 mod Q
#define ND 0xFC7FDFFF   // 1/(R-Q) mod R


/** @brief Dilithium signature key pair generation
 *
    @param RNG Random Number Generator handle
    @param SK secret key
    @param PK public key

 */
extern void DLTHM_keypair(csprng *,octet *SK,octet *PK);

/** @brief Dilithium create signature on a message 
 *
    @param SK secret key
    @param M the message to be signed
    @param SIG the output signature
    @return numbers of attempts needed to create signature

 */
extern int DLTHM_signature(octet *SK,octet *M, octet *SIG);

/** @brief Dilithium verify signature on a message 
 *
    @param PK public key key
    @param M the signed message
    @param SIG the signature
    @return true if signature is good, else false

 */
extern bool DLTHM_verify(octet *PK,octet *M,octet *SIG);

#endif
