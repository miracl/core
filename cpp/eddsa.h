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
 * @file ecdh.h
 * @author Mike Scott and Kealan McCusker
 * @date 2nd June 2015
 * @brief ECDH Header file for implementation of standard EC protocols
 *
 * declares functions
 *
 */

#ifndef EDDSA_ZZZ_H
#define EDDSA_ZZZ_H

#include "ecp_ZZZ.h"
//#include "ecdh_support.h"

using namespace core;

#define EGS_ZZZ MODBYTES_XXX  /**< ECC Group Size in bytes */
#define EFS_ZZZ MODBYTES_XXX  /**< ECC Field Size in bytes */

#define ECDH_OK                     0     /**< Function completed without error */
/*#define ECDH_DOMAIN_ERROR          -1*/
#define ECDH_INVALID_PUBLIC_KEY    -2	/**< Public Key is Invalid */
#define ECDH_ERROR                 -3	/**< ECDH Internal Error */
//#define ECDH_INVALID               -4	/**< ECDH Internal Error */
/*#define ECDH_DOMAIN_NOT_FOUND      -5
#define ECDH_OUT_OF_MEMORY         -6
#define ECDH_DIV_BY_ZERO           -7
#define ECDH_BAD_ASSUMPTION        -8*/


namespace ZZZ {

/* EDDSA primitives */
/**	@brief Generate an ECC public/private key pair
 *
	@param R is a pointer to a cryptographically secure random number generator
    @param b is the number of bits in the private key
	@param s the private key, an output internally randomly generated if R!=NULL, otherwise must be provided as an input
	@param W the output public key, which is s.G, where G is a fixed generator
	@return 0 or an error code
 */
extern int EDDSA_KEY_PAIR_GENERATE(csprng *R, int b, octet *s, octet *W);

}

#endif

