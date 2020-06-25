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
 * @file mpin.h
 * @author Mike Scott and Kealan McCusker
 * @date 2nd June 2015
 * @brief M-Pin Header file
 *
 * Allows some user configuration
 * defines structures
 * declares functions
 *
 */

#ifndef MPIN_ZZZ_H
#define MPIN_ZZZ_H

#include "pair4_ZZZ.h"
//#include "pbc_support.h"

using namespace core;


namespace ZZZ {

/* Field size is assumed to be greater than or equal to group size */

#define PGS_ZZZ MODBYTES_XXX  /**< MPIN Group Size */
#define PFS_ZZZ MODBYTES_XXX  /**< MPIN Field Size */

#define MPIN_OK             0   /**< Function completed without error */
#define MPIN_INVALID_POINT  -14	/**< Point is NOT on the curve */
#define MPIN_BAD_PIN        -19 /**< Bad PIN number entered */

#define MAXPIN 10000         /**< max PIN */
#define PBLEN 14             /**< max length of PIN in bits */

//#define PAS_ZZZ 24        /**< MPIN Symmetric Key Size 192 bits  */
//#define HASH_TYPE_MPIN_ZZZ SHA384   /**< Choose Hash function */

/* MPIN support functions */

/* MPIN primitives */

/**	@brief Encode a string to a curve point (in constant time)
 *
	@param DST is the Domain Separation Tag
    @param ID is the input string
    @param HID is the output point in G1
*/
void MPIN_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HID);

/**	@brief Extract a PIN number from a client secret
 *
	@param HID is the hashed-to-curve input client identity
	@param pin is an input PIN number
	@param CS is the client secret from which the PIN is to be extracted
	@return 0 or an error code
 */
int MPIN_EXTRACT_PIN(octet *HID, int pin, octet *CS);

/**	@brief Perform first pass of the client side of the 3-pass version of the M-Pin protocol
 *
	@param HID is the hashed-to-curve input client identity
	@param R is a pointer to a cryptographically secure random number generator
	@param x an output internally randomly generated if R!=NULL, otherwise must be provided as an input
	@param pin is the input PIN number
	@param T is the input M-Pin token (the client secret with PIN portion removed)
	@param S is the reconstructed client secret
	@param U is output = x.H(ID)
	@return 0 or an error code
 */
int MPIN_CLIENT_1(octet *HID, csprng *R, octet *x, int pin, octet *T, octet *S, octet *U);
/**	@brief Generate a random group element
 *
	@param R is a pointer to a cryptographically secure random number generator
	@param S is the output random octet
	@return 0 or an error code
 */
int MPIN_RANDOM_GENERATE(csprng *R, octet *S);
/**	@brief Perform second pass of the client side of the 3-pass version of the M-Pin protocol
 *
	@param x an input, a locally generated random number
	@param y an input random challenge from the server
	@param V on output = -(x+y).V
	@return 0 or an error code
 */
int MPIN_CLIENT_2(octet *x, octet *y, octet *V);

/**	@brief Perform third pass on the server side of the 3-pass version of the M-Pin protocol
 *
	@param HID is input H(ID), a hash of the client ID
	@param y is the input server's randomly generated challenge
	@param SS is the input server secret
	@param U is input from the client = x.H(ID)
	@param V is an input from the client
	@return 0 or an error code
 */
int MPIN_SERVER(octet *HID, octet *y, octet *SS, octet *U, octet *V);

/**	@brief Create a client secret in G1 from a master secret and the client ID
 *
	@param S is an input master secret
	@param HID is the input client identity hashed to point
	@param CS is the full client secret = s.H(ID)
	@return 0 or an error code
 */
int MPIN_GET_CLIENT_SECRET(octet *S, octet *HID, octet *CS);

/**	@brief Create a server secret in G2 from a master secret
 *
	@param S is an input master secret
	@param SS is the server secret = s.Q where Q is a fixed generator of G2
	@return 0 or an error code
 */
int MPIN_GET_SERVER_SECRET(octet *S, octet *SS);
}
#endif

