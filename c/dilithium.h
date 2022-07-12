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

/* DILITHIUM parameters */

//q= 8380417
#define DL_LGN 8
#define DL_DEGREE (1 << DL_LGN)
#define DL_PRIME 0x7fe001
#define DL_D 13
#define DL_TD (23-DL_D)

#define DL_ONE 0x3FFE00    // R mod Q
#define DL_COMBO 0xA3FA    // ONE*inv mod Q
#define DL_R2MODP 0x2419FF // R^2 mod Q
#define DL_ND 0xFC7FDFFF   // 1/(R-Q) mod R

#define DL_MAXLG 19
#define DL_MAXK 8     // could reduce these if not using highest security
#define DL_MAXL 7
#define DL_YBYTES (((DL_MAXLG+1)*DL_DEGREE)/8)

#define DL_SK_SIZE_2 (32*3+DL_DEGREE*(4*13+4*3+4*3)/8)
#define DL_PK_SIZE_2 ((4*DL_DEGREE*DL_TD)/8+32)
#define DL_SIG_SIZE_2 ((DL_DEGREE*4*(17+1))/8+80+4+32)

#define DL_SK_SIZE_3 (32*3+DL_DEGREE*(6*13+5*4+6*4)/8)
#define DL_PK_SIZE_3 ((6*DL_DEGREE*DL_TD)/8+32)
#define DL_SIG_SIZE_3 ((DL_DEGREE*5*(19+1))/8+55+6+32)

#define DL_SK_SIZE_5 (32*3+DL_DEGREE*(8*13+7*3+8*3)/8)
#define DL_PK_SIZE_5 ((8*DL_DEGREE*DL_TD)/8+32)
#define DL_SIG_SIZE_5 ((DL_DEGREE*7*(19+1))/8+75+8+32)

/** @brief Dilithium signature key pair generation
 *
    @param tau Random Numbers
    @param SK secret key
    @param PK public key

 */
extern void DLTHM_keypair_2(byte *tau,octet *SK,octet *PK);

/** @brief Dilithium create signature on a message 
 *
    @param SK secret key
    @param M the message to be signed
    @param SIG the output signature
    @return numbers of attempts needed to create signature

 */
extern int DLTHM_signature_2(octet *SK,octet *M, octet *SIG);

/** @brief Dilithium verify signature on a message 
 *
    @param PK public key key
    @param M the signed message
    @param SIG the signature
    @return true if signature is good, else false

 */
extern bool DLTHM_verify_2(octet *PK,octet *M,octet *SIG);


/** @brief Dilithium signature key pair generation
 *
    @param tau Random Numbers
    @param SK secret key
    @param PK public key

 */
extern void DLTHM_keypair_3(byte *tau,octet *SK,octet *PK);

/** @brief Dilithium create signature on a message 
 *
    @param SK secret key
    @param M the message to be signed
    @param SIG the output signature
    @return numbers of attempts needed to create signature

 */
extern int DLTHM_signature_3(octet *SK,octet *M, octet *SIG);

/** @brief Dilithium verify signature on a message 
 *
    @param PK public key key
    @param M the signed message
    @param SIG the signature
    @return true if signature is good, else false

 */
extern bool DLTHM_verify_3(octet *PK,octet *M,octet *SIG);


/** @brief Dilithium signature key pair generation
 *
    @param tau Random Numbers
    @param SK secret key
    @param PK public key

 */
extern void DLTHM_keypair_5(byte *tau,octet *SK,octet *PK);

/** @brief Dilithium create signature on a message 
 *
    @param SK secret key
    @param M the message to be signed
    @param SIG the output signature
    @return numbers of attempts needed to create signature

 */
extern int DLTHM_signature_5(octet *SK,octet *M, octet *SIG);

/** @brief Dilithium verify signature on a message 
 *
    @param PK public key key
    @param M the signed message
    @param SIG the signature
    @return true if signature is good, else false

 */
extern bool DLTHM_verify_5(octet *PK,octet *M,octet *SIG);

#endif
