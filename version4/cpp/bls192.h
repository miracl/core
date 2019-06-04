/*
Licensed to the Apache Software Foundation (ASF) under one
or more contributor license agreements.  See the NOTICE file
distributed with this work for additional information
regarding copyright ownership.  The ASF licenses this file
to you under the Apache License, Version 2.0 (the
"License"); you may not use this file except in compliance
with the License.  You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing,
software distributed under the License is distributed on an
"AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
KIND, either express or implied.  See the License for the
specific language governing permissions and limitations
under the License.
*/

/**
 * @file bls192.h
 * @author Mike Scott
 * @date 28th Novemebr 2018
 * @brief BLS Header file
 *
 * Allows some user configuration
 * defines structures
 * declares functions
 *
 */

#ifndef BLS192_ZZZ_H
#define BLS192_ZZZ_H

#include "pair192_ZZZ.h"

using namespace amcl;

namespace ZZZ {

/* Field size is assumed to be greater than or equal to group size */

#define BGS_ZZZ MODBYTES_XXX  /**< BLS Group Size */
#define BFS_ZZZ MODBYTES_XXX  /**< BLS Field Size */

#define BLS_OK           0  /**< Function completed without error */
#define BLS_FAIL		-1	/**< Point is NOT on the curve */

/* BLS API functions */

/**	@brief Initialise BLS
 *
 */
void BLS_INIT();

/**	@brief Generate Key Pair
 *
	@param RNG is a pointer to a cryptographically secure random number generator
	@param S on output a private key
	@param V on output a private key = S*G, where G is fixed generator
	@return BLS_OK
 */
int BLS_KEY_PAIR_GENERATE(csprng *RNG,octet* S,octet *W);

/**	@brief Calculate a signature
 *
	@param SIG the ouput signature
	@param m is the message to be signed
	@param S an input private key
	@return BLS_OK
 */
int BLS_SIGN(octet *SIG,char *m,octet *S);

/**	@brief Verify a signature
 *
	@param SIG an input signature
	@param m is the message whose signature is to be verified.
	@param W an public key
	@return BLS_OK if verified, otherwise BLS_FAIL
 */
int BLS_VERIFY(octet *SIG,char *m,octet *W);
}

#endif

