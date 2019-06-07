#ifndef FP16_YYY_H
#define FP16_YYY_H

#include "fp8_YYY.h"
#include "config_curve_ZZZ.h"


/**
	@brief FP16 Structure - towered over two FP8
*/

typedef struct
{
    FP8_YYY a; /**< real part of FP16 */
    FP8_YYY b; /**< imaginary part of FP16 */
} FP16_YYY;


/* FP16 prototypes */
/**	@brief Tests for FP16 equal to zero
 *
	@param x FP16 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP16_YYY_iszilch(FP16_YYY *x);
/**	@brief Tests for FP16 equal to unity
 *
	@param x FP16 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP16_YYY_isunity(FP16_YYY *x);
/**	@brief Tests for equality of two FP16s
 *
	@param x FP16 instance to be compared
	@param y FP16 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP16_YYY_equals(FP16_YYY *x,FP16_YYY *y);
/**	@brief Tests for FP16 having only a real part and no imaginary part
 *
	@param x FP16 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP16_YYY_isreal(FP16_YYY *x);
/**	@brief Initialise FP16 from two FP8s
 *
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
	@param b FP8 to form imaginary part of FP16
 */
extern void FP16_YYY_from_FP8s(FP16_YYY *x,FP8_YYY *a,FP8_YYY *b);
/**	@brief Initialise FP16 from single FP8
 *
	Imaginary part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
 */
extern void FP16_YYY_from_FP8(FP16_YYY *x,FP8_YYY *a);

/**	@brief Initialise FP16 from single FP8
 *
	real part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form imaginary part of FP16
 */
extern void FP16_YYY_from_FP8H(FP16_YYY *x,FP8_YYY *a);


/**	@brief Copy FP16 to another FP16
 *
	@param x FP16 instance, on exit = y
	@param y FP16 instance to be copied
 */
extern void FP16_YYY_copy(FP16_YYY *x,FP16_YYY *y);
/**	@brief Set FP16 to zero
 *
	@param x FP16 instance to be set to zero
 */
extern void FP16_YYY_zero(FP16_YYY *x);
/**	@brief Set FP16 to unity
 *
	@param x FP16 instance to be set to one
 */
extern void FP16_YYY_one(FP16_YYY *x);
/**	@brief Negation of FP16
 *
	@param x FP16 instance, on exit = -y
	@param y FP16 instance
 */
extern void FP16_YYY_neg(FP16_YYY *x,FP16_YYY *y);
/**	@brief Conjugation of FP16
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP16 instance, on exit = conj(y)
	@param y FP16 instance
 */
extern void FP16_YYY_conj(FP16_YYY *x,FP16_YYY *y);
/**	@brief Negative conjugation of FP16
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP16 instance, on exit = -conj(y)
	@param y FP16 instance
 */
extern void FP16_YYY_nconj(FP16_YYY *x,FP16_YYY *y);
/**	@brief addition of two FP16s
 *
	@param x FP16 instance, on exit = y+z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_YYY_add(FP16_YYY *x,FP16_YYY *y,FP16_YYY *z);
/**	@brief subtraction of two FP16s
 *
	@param x FP16 instance, on exit = y-z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_YYY_sub(FP16_YYY *x,FP16_YYY *y,FP16_YYY *z);
/**	@brief Multiplication of an FP16 by an FP8
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP8 multiplier
 */
extern void FP16_YYY_pmul(FP16_YYY *x,FP16_YYY *y,FP8_YYY *a);

/**	@brief Multiplication of an FP16 by an FP2
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP2 multiplier
 */
extern void FP16_YYY_qmul(FP16_YYY *x,FP16_YYY *y,FP2_YYY *a);

/**	@brief Multiplication of an FP16 by an FP
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP multiplier
 */
extern void FP16_YYY_tmul(FP16_YYY *x,FP16_YYY *y,FP_YYY *a);

/**	@brief Multiplication of an FP16 by a small integer
 *
	@param x FP16 instance, on exit = y*i
	@param y FP16 instance
	@param i an integer
 */
extern void FP16_YYY_imul(FP16_YYY *x,FP16_YYY *y,int i);
/**	@brief Squaring an FP16
 *
	@param x FP16 instance, on exit = y^2
	@param y FP16 instance
 */
extern void FP16_YYY_sqr(FP16_YYY *x,FP16_YYY *y);
/**	@brief Multiplication of two FP16s
 *
	@param x FP16 instance, on exit = y*z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_YYY_mul(FP16_YYY *x,FP16_YYY *y,FP16_YYY *z);
/**	@brief Inverting an FP16
 *
	@param x FP16 instance, on exit = 1/y
	@param y FP16 instance
 */
extern void FP16_YYY_inv(FP16_YYY *x,FP16_YYY *y);
/**	@brief Formats and outputs an FP16 to the console
 *
	@param x FP16 instance to be printed
 */
extern void FP16_YYY_output(FP16_YYY *x);
/**	@brief Formats and outputs an FP16 to the console in raw form (for debugging)
 *
	@param x FP16 instance to be printed
 */
extern void FP16_YYY_rawoutput(FP16_YYY *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP16_YYY_times_i(FP16_YYY *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^2*x
 */
extern void FP16_YYY_times_i2(FP16_YYY *x);

/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^4*x
 */
extern void FP16_YYY_times_i4(FP16_YYY *x);


/**	@brief Normalises the components of an FP16
 *
	@param x FP16 instance to be normalised
 */
extern void FP16_YYY_norm(FP16_YYY *x);
/**	@brief Reduces all components of possibly unreduced FP16 mod Modulus
 *
	@param x FP16 instance, on exit reduced mod Modulus
 */
extern void FP16_YYY_reduce(FP16_YYY *x);
/**	@brief Raises an FP16 to the power of a BIG
 *
	@param x FP16 instance, on exit = y^b
	@param y FP16 instance
	@param b BIG number
 */
extern void FP16_YYY_pow(FP16_YYY *x,FP16_YYY *y,BIG_XXX b);
/**	@brief Raises an FP16 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP16 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP16_YYY_frob(FP16_YYY *x,FP2_YYY *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP16 instance, on exit = w*x-conj(x)*y+z
	@param w FP16 instance
	@param x FP16 instance
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_YYY_xtr_A(FP16_YYY *r,FP16_YYY *w,FP16_YYY *x,FP16_YYY *y,FP16_YYY *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP16 instance, on exit = x^2-2*conj(x)
	@param x FP16 instance
 */
extern void FP16_YYY_xtr_D(FP16_YYY *r,FP16_YYY *x);
/**	@brief Calculates FP16 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP16 instance, on exit = trace(w^b)
	@param x FP16 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP16_YYY_xtr_pow(FP16_YYY *r,FP16_YYY *x,BIG_XXX b);
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
extern void FP16_YYY_xtr_pow2(FP16_YYY *r,FP16_YYY *c,FP16_YYY *d,FP16_YYY *e,FP16_YYY *f,BIG_XXX a,BIG_XXX b);

/**	@brief Conditional copy of FP16 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP16 instance, set to y if s!=0
	@param y another FP16 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP16_YYY_cmove(FP16_YYY *x,FP16_YYY *y,int s);


#endif

