#ifndef RANDOM_H
#define RANDOM_H

#include "amcl.h"

namespace amcl {

/**	@brief Initialise a random number generator
 *
	@param R is a pointer to a cryptographically secure random number generator
	@param S is an input truly random seed value
 */
extern void CREATE_CSPRNG(csprng *R,octet *S);
/**	@brief Kill a random number generator
 *
	Deletes all internal state
	@param R is a pointer to a cryptographically secure random number generator
 */
extern void KILL_CSPRNG(csprng *R);

}

#endif

