#ifndef FP24_YYY_H
#define FP24_YYY_H

#include "fp8_YYY.h"

/**
	@brief FP12 Structure - towered over three FP8
*/

typedef struct
{
    FP8_YYY a; /**< first part of FP12 */
    FP8_YYY b; /**< second part of FP12 */
    FP8_YYY c; /**< third part of FP12 */
	int type;
} FP24_YYY;

extern const BIG_XXX Fra_YYY; /**< real part of BN curve Frobenius Constant */
extern const BIG_XXX Frb_YYY; /**< imaginary part of BN curve Frobenius Constant */

/* FP24 prototypes */
/**	@brief Tests for FP24 equal to zero
 *
	@param x FP24 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP24_YYY_iszilch(FP24_YYY *x);
/**	@brief Tests for FP24 equal to unity
 *
	@param x FP24 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP24_YYY_isunity(FP24_YYY *x);
/**	@brief Copy FP24 to another FP24
 *
	@param x FP24 instance, on exit = y
	@param y FP24 instance to be copied
 */
extern void FP24_YYY_copy(FP24_YYY *x,FP24_YYY *y);
/**	@brief Set FP24 to unity
 *
	@param x FP24 instance to be set to one
 */
extern void FP24_YYY_one(FP24_YYY *x);

/**	@brief Set FP24 to zero
 *
	@param x FP24 instance to be set to zero
 */
extern void FP24_YYY_zero(FP24_YYY *x);

/**	@brief Tests for equality of two FP24s
 *
	@param x FP24 instance to be compared
	@param y FP24 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP24_YYY_equals(FP24_YYY *x,FP24_YYY *y);
/**	@brief Conjugation of FP24
 *
	If y=(a,b,c) (where a,b,c are its three FP8 components) on exit x=(conj(a),-conj(b),conj(c))
	@param x FP24 instance, on exit = conj(y)
	@param y FP24 instance
 */
extern void FP24_YYY_conj(FP24_YYY *x,FP24_YYY *y);
/**	@brief Initialise FP24 from single FP8
 *
	Sets first FP8 component of an FP24, other components set to zero
	@param x FP24 instance to be initialised
	@param a FP8 to form first part of FP8
 */
extern void FP24_YYY_from_FP8(FP24_YYY *x,FP8_YYY *a);
/**	@brief Initialise FP24 from three FP8s
 *
	@param x FP24 instance to be initialised
	@param a FP8 to form first part of FP24
	@param b FP8 to form second part of FP24
	@param c FP8 to form third part of FP24
 */
extern void FP24_YYY_from_FP8s(FP24_YYY *x,FP8_YYY *a,FP8_YYY* b,FP8_YYY *c);
/**	@brief Fast Squaring of an FP24 in "unitary" form
 *
	@param x FP24 instance, on exit = y^2
	@param y FP8 instance, must be unitary
 */
extern void FP24_YYY_usqr(FP24_YYY *x,FP24_YYY *y);
/**	@brief Squaring an FP24
 *
	@param x FP24 instance, on exit = y^2
	@param y FP24 instance
 */
extern void FP24_YYY_sqr(FP24_YYY *x,FP24_YYY *y);
/**	@brief Fast multiplication of two sparse FP24s that arises from ATE pairing line functions
 *
	@param x FP24 instance, on exit = x*y
	@param y FP24 instance, of special form
 */
extern void FP24_YYY_smul(FP24_YYY *x,FP24_YYY *y);

/**	@brief Fast multiplication of what may be sparse multiplicands
 *
	@param x FP24 instance, on exit = x*y
	@param y FP24 instance, of special form
 */
extern void FP24_YYY_ssmul(FP24_YYY *x,FP24_YYY *y);
/**	@brief Full unconditional Multiplication of two FP24s
 *
	@param x FP24 instance, on exit = x*y
	@param y FP24 instance, the multiplier
 */
extern void FP24_YYY_mul(FP24_YYY *x,FP24_YYY *y);

/**	@brief Inverting an FP24
 *
	@param x FP24 instance, on exit = 1/y
	@param y FP24 instance
 */
extern void FP24_YYY_inv(FP24_YYY *x,FP24_YYY *y);
/**	@brief Raises an FP24 to the power of a BIG
 *
	@param r FP24 instance, on exit = y^b
	@param x FP24 instance
	@param b BIG number
 */
extern void FP24_YYY_pow(FP24_YYY *r,FP24_YYY *x,BIG_XXX b);

//extern void FP24_ppow(FP24 *r,FP24 *x,BIG b);

/**	@brief Raises an FP24 instance x to a small integer power, side-channel resistant
 *
	@param x FP24 instance, on exit = x^i
	@param i small integer exponent
	@param b maximum number of bits in exponent
 */
extern void FP24_YYY_pinpow(FP24_YYY *x,int i,int b);

/**	@brief Raises an FP24 instance x to a BIG power, compressed to FP8 
 *
	@param c FP8 instance, on exit = x^(e mod r) as FP8
	@param x FP24 input
	@param e BIG exponent
	@param r BIG group order
 */
extern void FP24_YYY_compow(FP8_YYY *c,FP24_YYY *x,BIG_XXX e,BIG_XXX r);

/**	@brief Calculate Pi x[i]^b[i] for i=0 to 7, side-channel resistant
 *
	@param r FP24 instance, on exit = Pi x[i]^b[i] for i=0 to 7
	@param x FP24 array with 4 FP24s
	@param b BIG array of 4 exponents
 */
extern void FP24_YYY_pow8(FP24_YYY *r,FP24_YYY *x,BIG_XXX *b);


/**	@brief Raises an FP24 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP24 instance, on exit = x^p^n
	@param f FP2 precalculated Frobenius constant
	@param n power of p
 */
extern void FP24_YYY_frob(FP24_YYY *x,FP2_YYY *f,int n);

/**	@brief Reduces all components of possibly unreduced FP24 mod Modulus
 *
	@param x FP24 instance, on exit reduced mod Modulus
 */
extern void FP24_YYY_reduce(FP24_YYY *x);
/**	@brief Normalises the components of an FP24
 *
	@param x FP24 instance to be normalised
 */
extern void FP24_YYY_norm(FP24_YYY *x);
/**	@brief Formats and outputs an FP24 to the console
 *
	@param x FP24 instance to be printed
 */
extern void FP24_YYY_output(FP24_YYY *x);
/**	@brief Formats and outputs an FP24 instance to an octet string
 *
	Serializes the components of an FP24 to big-endian base 256 form.
	@param S output octet string
	@param x FP24 instance to be converted to an octet string
 */
extern void FP24_YYY_toOctet(octet *S,FP24_YYY *x);
/**	@brief Creates an FP24 instance from an octet string
 *
	De-serializes the components of an FP24 to create an FP24 from big-endian base 256 components.
	@param x FP24 instance to be created from an octet string
	@param S input octet string

 */
extern void FP24_YYY_fromOctet(FP24_YYY *x,octet *S);
/**	@brief Calculate the trace of an FP24
 *
	@param t FP8 trace of x, on exit = tr(x)
	@param x FP24 instance

 */
extern void FP24_YYY_trace(FP8_YYY *t,FP24_YYY *x);

/**	@brief Conditional copy of FP24_YYY number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP24_YYY instance, set to y if s!=0
	@param y another FP24_YYY instance
	@param s copy only takes place if not equal to 0
 */
extern void FP24_YYY_cmove(FP24_YYY *x,FP24_YYY *y,int s);

#endif
