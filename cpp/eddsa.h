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

/**
 * @file eddsa.h
 * @author Mike Scott
 * @date 14th April 2023
 * @brief EDDSA Header file
 *
 * declares functions
 *
 */

#ifndef EDDSA_ZZZ_H
#define EDDSA_ZZZ_H

#include "ecp_ZZZ.h"

using namespace core;

#define EGS_ZZZ MODBYTES_XXX  /**< ECC Group Size in bytes */
#define EFS_ZZZ MODBYTES_XXX  /**< ECC Field Size in bytes */

#define EDDSA_OK                     0     /**< Function completed without error */
#define EDDSA_INVALID_PUBLIC_KEY    -2	/**< Public Key is Invalid */
#define EDDSA_ERROR                 -3	/**< ECDH Internal Error */

namespace ZZZ {

/* EDDSA primitives */
/**	@brief Generate an ECC public/private key pair
 *
	@param R is a pointer to a cryptographically secure random number generator
	@param D the private key, an output internally randomly generated if R!=NULL, otherwise must be provided as an input
	@param Q the output public key, which is D.G, where G is a fixed generator
	@return 0 or an error code
 */
extern int EDDSA_KEY_PAIR_GENERATE(csprng *R, octet *D, octet *Q);

/**	@brief Generate signature on a message
 *
    @param ph true for pre-hash version, default to false
	@param D the private key
	@param context is an octet string (or NULL)
	@param M is the message to be signed
    @param SIG is the output signature
	@return 0 or an error code
 */
extern int EDDSA_SIGNATURE(bool ph,octet *D, octet *context,octet *M,octet *SIG);

/**	@brief Verify signature on a message
 *
    @param ph true for pre-hash version, default to false
	@param Q the public key
	@param context is an octet string (or NULL)
	@param M is the signed message
    @param SIG is the intput signature
	@return true only if signature is good, else false
 */
extern bool EDDSA_VERIFY(bool ph,octet *Q,octet *context,octet *M,octet *SIG);

}

#endif

