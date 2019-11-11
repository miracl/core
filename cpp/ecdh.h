/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
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

#ifndef ECDH_ZZZ_H
#define ECDH_ZZZ_H

#include "ecp_ZZZ.h"
//#include "ecdh_support.h"

using namespace core;


/*** START OF USER CONFIGURABLE SECTION -  ***/

/*** START OF USER CONFIGURABLE SECTION -  ***/


/*** END OF USER CONFIGURABLE SECTION ***/

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

/* ECDH primitives */
/**	@brief Generate an ECC public/private key pair
 *
	@param R is a pointer to a cryptographically secure random number generator
	@param s the private key, an output internally randomly generated if R!=NULL, otherwise must be provided as an input
	@param W the output public key, which is s.G, where G is a fixed generator
	@return 0 or an error code
 */
extern int  ECP_KEY_PAIR_GENERATE(csprng *R, octet *s, octet *W);
/**	@brief Validate an ECC public key
 *
	@param W the input public key to be validated
	@return 0 if public key is OK, or an error code
 */
extern int  ECP_PUBLIC_KEY_VALIDATE(octet *W);

/* ECDH primitives */

/**	@brief Generate Diffie-Hellman shared key
 *
	IEEE-1363 Diffie-Hellman shared secret calculation
	@param s is the input private key,
	@param W the input public key of the other party
	@param K the output shared key, in fact the x-coordinate of s.W
	@return 0 or an error code
 */
extern int ECP_SVDP_DH(octet *s, octet *W, octet *K);
/*extern int ECPSVDP_DHC(octet *,octet *,int,octet *);*/

/*#if CURVETYPE!=MONTGOMERY */
/* ECIES functions */
/*#if CURVETYPE!=MONTGOMERY */
/* ECIES functions */
/**	@brief ECIES Encryption
 *
	IEEE-1363 ECIES Encryption
	@param P1 input Key Derivation parameters
	@param P2 input Encoding parameters
	@param R is a pointer to a cryptographically secure random number generator
	@param W the input public key of the recieving party
	@param M is the plaintext message to be encrypted
	@param len the length of the HMAC tag
	@param V component of the output ciphertext
	@param C the output ciphertext
	@param T the output HMAC tag, part of the ciphertext
 */
extern void ECP_ECIES_ENCRYPT(int hlen, octet *P1, octet *P2, csprng *R, octet *W, octet *M, int len, octet *V, octet *C, octet *T);
/**	@brief ECIES Decryption
 *
	IEEE-1363 ECIES Decryption
    @param hlen is hash output length
	@param P1 input Key Derivation parameters
	@param P2 input Encoding parameters
	@param V component of the input ciphertext
	@param C the input ciphertext
	@param T the input HMAC tag, part of the ciphertext
	@param U the input private key for decryption
	@param M the output plaintext message
	@return 1 if successful, else 0
 */
extern int ECP_ECIES_DECRYPT(int hlen, octet *P1, octet *P2, octet *V, octet *C, octet *T, octet *U, octet *M);

/* ECDSA functions */
/**	@brief ECDSA Signature
 *
	IEEE-1363 ECDSA Signature
    @param hlen is hash output length
	@param R is a pointer to a cryptographically secure random number generator
    @param k Ephemeral key. This value is used when R=NULL
	@param s the input private signing key
	@param M the input message to be signed
	@param c component of the output signature
	@param d component of the output signature

 */
extern int ECP_SP_DSA(int hlen, csprng *R, octet *k, octet *s, octet *M, octet *c, octet *d);
/**	@brief ECDSA Signature Verification
 *
	IEEE-1363 ECDSA Signature Verification
    @param hlen is hash output length
	@param W the input public key
	@param M the input message
	@param c component of the input signature
	@param d component of the input signature
	@return 0 or an error code
 */
extern int ECP_VP_DSA(int hlen, octet *W, octet *M, octet *c, octet *d);
/*#endif*/
}

#endif

