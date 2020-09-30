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

#ifndef FP16_YYY_H
#define FP16_YYY_H

#include "fp8_YYY.h"
#include "config_curve_ZZZ.h"

using namespace core;

namespace YYY {

/**
	@brief FP16 Structure - towered over two FP4
*/

typedef struct
{
    FP8 a; /**< real part of FP16 */
    FP8 b; /**< imaginary part of FP16 */
} FP16;


/* FP16 prototypes */
/**	@brief Tests for FP16 equal to zero
 *
	@param x FP16 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP16_iszilch(FP16 *x);
/**	@brief Tests for FP16 equal to unity
 *
	@param x FP16 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP16_isunity(FP16 *x);
/**	@brief Tests for equality of two FP16s
 *
	@param x FP16 instance to be compared
	@param y FP16 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP16_equals(FP16 *x, FP16 *y);

/**	@brief Serialize in FP16  
 *
    @param b buffer for output
	@param x FP16 number to be serialized
 */
extern void FP16_toBytes(char *b,FP16 *x);
/**	@brief Serialize out FP16  
 *
	@param x FP16 number to be serialized
    @param b buffer for input
 */
extern void FP16_fromBytes(FP16 *x,char *b);

/**	@brief Tests for FP16 having only a real part and no imaginary part
 *
	@param x FP16 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP16_isreal(FP16 *x);
/**	@brief Initialise FP16 from two FP8s
 *
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
	@param b FP8 to form imaginary part of FP16
 */
extern void FP16_from_FP8s(FP16 *x, FP8 *a, FP8 *b);
/**	@brief Initialise FP16 from single FP8
 *
	Imaginary part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
 */
extern void FP16_from_FP8(FP16 *x, FP8 *a);

/**	@brief Initialise FP16 from single FP8
 *
	real part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form imaginary part of FP16
 */
extern void FP16_from_FP8H(FP16 *x, FP8 *a);


/**	@brief Copy FP16 to another FP16
 *
	@param x FP16 instance, on exit = y
	@param y FP16 instance to be copied
 */
extern void FP16_copy(FP16 *x, FP16 *y);
/**	@brief Set FP16 to zero
 *
	@param x FP16 instance to be set to zero
 */
extern void FP16_zero(FP16 *x);
/**	@brief Set FP16 to unity
 *
	@param x FP16 instance to be set to one
 */
extern void FP16_one(FP16 *x);
/**	@brief Negation of FP16
 *
	@param x FP16 instance, on exit = -y
	@param y FP16 instance
 */
extern void FP16_neg(FP16 *x, FP16 *y);
/**	@brief Conjugation of FP16
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP16 instance, on exit = conj(y)
	@param y FP16 instance
 */
extern void FP16_conj(FP16 *x, FP16 *y);
/**	@brief Negative conjugation of FP16
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP16 instance, on exit = -conj(y)
	@param y FP16 instance
 */
extern void FP16_nconj(FP16 *x, FP16 *y);
/**	@brief addition of two FP16s
 *
	@param x FP16 instance, on exit = y+z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_add(FP16 *x, FP16 *y, FP16 *z);
/**	@brief subtraction of two FP16s
 *
	@param x FP16 instance, on exit = y-z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_sub(FP16 *x, FP16 *y, FP16 *z);
/**	@brief Multiplication of an FP16 by an FP8
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP4 multiplier
 */
extern void FP16_pmul(FP16 *x, FP16 *y, FP8 *a);

/**	@brief Multiplication of an FP16 by an FP2
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP2 multiplier
 */
extern void FP16_qmul(FP16 *x, FP16 *y, FP2 *a);


/**	@brief Multiplication of an FP16 by an FP
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP multiplier
 */
extern void FP16_tmul(FP16 *x, FP16 *y, FP *a);



/**	@brief Multiplication of an FP16 by a small integer
 *
	@param x FP16 instance, on exit = y*i
	@param y FP16 instance
	@param i an integer
 */
extern void FP16_imul(FP16 *x, FP16 *y, int i);
/**	@brief Squaring an FP16
 *
	@param x FP16 instance, on exit = y^2
	@param y FP16 instance
 */
extern void FP16_sqr(FP16 *x, FP16 *y);
/**	@brief Multiplication of two FP16s
 *
	@param x FP16 instance, on exit = y*z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_mul(FP16 *x, FP16 *y, FP16 *z);
/**	@brief Inverting an FP16
 *
	@param x FP16 instance, on exit = 1/y
	@param y FP16 instance
 */
extern void FP16_inv(FP16 *x, FP16 *y);
/**	@brief Formats and outputs an FP16 to the console
 *
	@param x FP16 instance to be printed
 */
extern void FP16_output(FP16 *x);
/**	@brief Formats and outputs an FP16 to the console in raw form (for debugging)
 *
	@param x FP16 instance to be printed
 */
extern void FP16_rawoutput(FP16 *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP16_times_i(FP16 *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^2*x
 */
extern void FP16_times_i2(FP16 *x);

/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^4*x
 */
extern void FP16_times_i4(FP16 *x);


/**	@brief Normalises the components of an FP16
 *
	@param x FP16 instance to be normalised
 */
extern void FP16_norm(FP16 *x);
/**	@brief Reduces all components of possibly unreduced FP16 mod Modulus
 *
	@param x FP16 instance, on exit reduced mod Modulus
 */
extern void FP16_reduce(FP16 *x);
/**	@brief Raises an FP16 to the power of a BIG
 *
	@param x FP16 instance, on exit = y^b
	@param y FP16 instance
	@param b BIG number
 */
extern void FP16_pow(FP16 *x, FP16 *y, XXX::BIG b);
/**	@brief Raises an FP16 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP16 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP16_frob(FP16 *x, FP2 *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP16 instance, on exit = w*x-conj(x)*y+z
	@param w FP16 instance
	@param x FP16 instance
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_xtr_A(FP16 *r, FP16 *w, FP16 *x, FP16 *y, FP16 *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP16 instance, on exit = x^2-2*conj(x)
	@param x FP16 instance
 */
extern void FP16_xtr_D(FP16 *r, FP16 *x);
/**	@brief Calculates FP16 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP16 instance, on exit = trace(w^b)
	@param x FP16 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP16_xtr_pow(FP16 *r, FP16 *x, XXX::BIG b);
/**	@brief Calculates FP16 trace of c^a.d^b, where c and d are derived from FP16 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP16 instance, on exit = trace(c^a.d^b)
	@param c FP16 instance, trace of an FP12
	@param d FP16 instance, trace of an FP12
	@param e FP16 instance, trace of an FP12
	@param f FP16 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP16_xtr_pow2(FP16 *r, FP16 *c, FP16 *d, FP16 *e, FP16 *f, XXX::BIG a, XXX::BIG b);

/**	@brief Conditional copy of FP16 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP16 instance, set to y if s!=0
	@param y another FP16 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP16_cmove(FP16 *x, FP16 *y, int s);

}

#endif

