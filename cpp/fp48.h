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

#ifndef FP48_YYY_H
#define FP48_YYY_H

#include "fp16_YYY.h"

using namespace core;

namespace YYY {
/**
	@brief FP12 Structure - towered over three FP16
*/

typedef struct
{
    FP16 a; /**< first part of FP12 */
    FP16 b; /**< second part of FP12 */
    FP16 c; /**< third part of FP12 */
    int type;
} FP48;

extern const XXX::BIG Fra; /**< real part of BN curve Frobenius Constant */
extern const XXX::BIG Frb; /**< imaginary part of BN curve Frobenius Constant */

/* FP48 prototypes */
/**	@brief Tests for FP48 equal to zero
 *
	@param x FP48 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP48_iszilch(FP48 *x);
/**	@brief Tests for FP48 equal to unity
 *
	@param x FP48 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP48_isunity(FP48 *x);
/**	@brief Copy FP48 to another FP48
 *
	@param x FP48 instance, on exit = y
	@param y FP48 instance to be copied
 */
extern void FP48_copy(FP48 *x, FP48 *y);
/**	@brief Set FP48 to unity
 *
	@param x FP48 instance to be set to one
 */
extern void FP48_one(FP48 *x);

/**	@brief Set FP48 to zero
 *
	@param x FP48 instance to be set to zero
 */
extern void FP48_zero(FP48 *x);

/**	@brief Tests for equality of two FP48s
 *
	@param x FP48 instance to be compared
	@param y FP48 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP48_equals(FP48 *x, FP48 *y);
/**	@brief Conjugation of FP48
 *
	If y=(a,b,c) (where a,b,c are its three FP16 components) on exit x=(conj(a),-conj(b),conj(c))
	@param x FP48 instance, on exit = conj(y)
	@param y FP48 instance
 */
extern void FP48_conj(FP48 *x, FP48 *y);
/**	@brief Initialise FP48 from single FP16
 *
	Sets first FP16 component of an FP48, other components set to zero
	@param x FP48 instance to be initialised
	@param a FP16 to form first part of FP48
 */
extern void FP48_from_FP16(FP48 *x, FP16 *a);
/**	@brief Initialise FP48 from three FP16s
 *
	@param x FP48 instance to be initialised
	@param a FP16 to form first part of FP48
	@param b FP16 to form second part of FP48
	@param c FP16 to form third part of FP48
 */
extern void FP48_from_FP16s(FP48 *x, FP16 *a, FP16* b, FP16 *c);
/**	@brief Fast Squaring of an FP48 in "unitary" form
 *
	@param x FP48 instance, on exit = y^2
	@param y FP16 instance, must be unitary
 */
extern void FP48_usqr(FP48 *x, FP48 *y);
/**	@brief Squaring an FP48
 *
	@param x FP48 instance, on exit = y^2
	@param y FP48 instance
 */
extern void FP48_sqr(FP48 *x, FP48 *y);
/**	@brief Fast multiplication of two sparse FP24s that arises from ATE pairing line functions
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, of special form
 */
extern void FP48_smul(FP48 *x, FP48 *y);

/**	@brief Fast multiplication of what may be sparse multiplicands
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, of special form
 */
extern void FP48_ssmul(FP48 *x, FP48 *y);
/**	@brief Full unconditional Multiplication of two FP24s
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, the multiplier
 */
extern void FP48_mul(FP48 *x, FP48 *y);
/**	@brief Inverting an FP48
 *
	@param x FP48 instance, on exit = 1/y
	@param y FP48 instance
 */
extern void FP48_inv(FP48 *x, FP48 *y);
/**	@brief Raises an FP48 to the power of a BIG
 *
	@param r FP48 instance, on exit = y^b
	@param x FP48 instance
	@param b BIG number
 */
extern void FP48_pow(FP48 *r, FP48 *x, XXX::BIG b);

//extern void FP48_ppow(FP48 *r,FP48 *x,XXX::BIG b);

/**	@brief Raises an FP48 instance x to a small integer power, side-channel resistant
 *
	@param x FP48 instance, on exit = x^i
	@param i small integer exponent
	@param b maximum number of bits in exponent
 */
extern void FP48_pinpow(FP48 *x, int i, int b);

/**	@brief Raises an FP48 instance x to a BIG power, compressed to FP16
 *
	@param c FP16 instance, on exit = x^(e mod r) as FP16
	@param x FP48 input
	@param e BIG exponent
	@param r BIG group order
 */
extern void FP48_compow(FP16 *c, FP48 *x, XXX::BIG e, XXX::BIG r);

/**	@brief Calculate Pi x[i]^b[i] for i=0 to 15, side-channel resistant
 *
	@param r FP48 instance, on exit = Pi x[i]^b[i] for i=0 to 15
	@param x FP48 array with 16 FP48s
	@param b BIG array of 16 exponents
 */
extern void FP48_pow16(FP48 *r, FP48 *x, XXX::BIG *b);


/**	@brief Raises an FP48 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP48 instance, on exit = x^p^n
	@param f FP2 precalculated Frobenius constant
	@param n power of p
 */
extern void FP48_frob(FP48 *x, FP2 *f, int n);

/**	@brief Reduces all components of possibly unreduced FP48 mod Modulus
 *
	@param x FP48 instance, on exit reduced mod Modulus
 */
extern void FP48_reduce(FP48 *x);
/**	@brief Normalises the components of an FP48
 *
	@param x FP48 instance to be normalised
 */
extern void FP48_norm(FP48 *x);
/**	@brief Formats and outputs an FP48 to the console
 *
	@param x FP48 instance to be printed
 */
extern void FP48_output(FP48 *x);
/**	@brief Formats and outputs an FP48 instance to an octet string
 *
	Serializes the components of an FP48 to big-endian base 256 form.
	@param S output octet string
	@param x FP48 instance to be converted to an octet string
 */
extern void FP48_toOctet(octet *S, FP48 *x);
/**	@brief Creates an FP48 instance from an octet string
 *
	De-serializes the components of an FP48 to create an FP48 from big-endian base 256 components.
	@param x FP48 instance to be created from an octet string
	@param S input octet string

 */
extern void FP48_fromOctet(FP48 *x, octet *S);
/**	@brief Calculate the trace of an FP48
 *
	@param t FP16 trace of x, on exit = tr(x)
	@param x FP48 instance

 */
extern void FP48_trace(FP16 *t, FP48 *x);

/**	@brief Conditional copy of FP48 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP48 instance, set to y if s!=0
	@param y another FP48 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP48_cmove(FP48 *x, FP48 *y, int s);
}
#endif
