#ifndef RSA_SUPPORT_H
#define RSA_SUPPORT_H

#include "amcl.h"

namespace amcl {

#define MAX_RSA_BYTES 1920 // Maximum of 4096

/**	@brief RSA Key Pair Generator
 *
	@param R is a pointer to a cryptographically secure random number generator
	@param e the encryption exponent
	@param PRIV the output RSA private key
	@param PUB the output RSA public key
        @param P Input prime number. Used when R is equal to NULL for testing
        @param Q Inpuy prime number. Used when R is equal to NULL for testing
 */
/**	@brief PKCS V1.5 padding of a message prior to RSA signature
 *
	@param h is the hash type
	@param M is the input message
	@param W is the output encoding, ready for RSA signature
	@return 1 if OK, else 0
 */
extern int PKCS15(int h,octet *M,octet *W);
/**	@brief OAEP padding of a message prior to RSA encryption
 *
	@param h is the hash type
	@param M is the input message
	@param R is a pointer to a cryptographically secure random number generator
	@param P are input encoding parameter string (could be NULL)
	@param F is the output encoding, ready for RSA encryption
	@return 1 if OK, else 0
 */
extern int	OAEP_ENCODE(int h,octet *M,csprng *R,octet *P,octet *F);
/**	@brief OAEP unpadding of a message after RSA decryption
 *
	Unpadding is done in-place
	@param h is the hash type
	@param P are input encoding parameter string (could be NULL)
	@param F is input padded message, unpadded on output
	@return 1 if OK, else 0
 */
extern int  OAEP_DECODE(int h,octet *P,octet *F);

}

#endif