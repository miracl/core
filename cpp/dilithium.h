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
#define DL_LGN 8
#define DL_DEGREE (1 << DL_LGN)
#define DL_PRIME 0x7fe001
#define DL_D 13
#define DL_TD (23-DL_D)

// These values change for alternate security levels
// Dilithium 3
#define DL_WC 49
#define DL_LG 19
#define DL_GAMMA1 ((sign32)1<<DL_LG)
#define DL_RR 32
#define DL_GAMMA2 (DL_PRIME-1)/DL_RR
#define DL_K 6
#define DL_L 5
#define DL_ETA 4
#define DL_LG2ETA1 4  // lg(2*ETA+1) rounded up
#define DL_BETA 196
#define DL_OMEGA 55

#define DL_YBYTES ((DL_LG+1)*DL_DEGREE)/8
#define DL_W1B 4
#define DL_ONE 0x3FFE00    // R mod Q
#define DL_COMBO 0xA3FA    // ONE*inv mod Q
#define DL_R2MODP 0x2419FF // R^2 mod Q
#define DL_ND 0xFC7FDFFF   // 1/(R-Q) mod R

#define DL_SK_LEN (32*3+DL_DEGREE*(DL_K*DL_D+DL_L*DL_LG2ETA1+DL_K*DL_LG2ETA1)/8)
#define DL_PK_LEN ((DL_K*DL_DEGREE*DL_TD)/8+32)
#define DL_SIG_LEN ((DL_DEGREE*DL_L*(DL_LG+1))/8+DL_OMEGA+DL_K+32)

namespace core {

/** @brief Dilithium signature key pair generation
 *
    @param tau Random Numbers
    @param SK secret key
    @param PK public key

 */
extern void DLTHM_keypair(byte *tau,octet *SK,octet *PK);

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
}

#endif
