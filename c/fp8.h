#ifndef FP8_YYY_H
#define FP8_YYY_H

#include "fp4_YYY.h"
#include "config_curve_ZZZ.h"


/**
	@brief FP8 Structure - towered over two FP4
*/

typedef struct
{
    FP4_YYY a; /**< real part of FP8 */
    FP4_YYY b; /**< imaginary part of FP8 */
} FP8_YYY;


/* FP8 prototypes */
/**	@brief Tests for FP8 equal to zero
 *
	@param x FP8 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP8_YYY_iszilch(FP8_YYY *x);
/**	@brief Tests for FP8 equal to unity
 *
	@param x FP8 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP8_YYY_isunity(FP8_YYY *x);
/**	@brief Tests for equality of two FP8s
 *
	@param x FP8 instance to be compared
	@param y FP8 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP8_YYY_equals(FP8_YYY *x,FP8_YYY *y);
/**	@brief Tests for FP8 having only a real part and no imaginary part
 *
	@param x FP8 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP8_YYY_isreal(FP8_YYY *x);
/**	@brief Initialise FP8 from two FP4s
 *
	@param x FP8 instance to be initialised
	@param a FP4 to form real part of FP8
	@param b FP4 to form imaginary part of FP8
 */
extern void FP8_YYY_from_FP4s(FP8_YYY *x,FP4_YYY *a,FP4_YYY *b);
/**	@brief Initialise FP8 from single FP4
 *
	Imaginary part is set to zero
	@param x FP8 instance to be initialised
	@param a FP4 to form real part of FP8
 */
extern void FP8_YYY_from_FP4(FP8_YYY *x,FP4_YYY *a);

/**	@brief Initialise FP8 from single FP4
 *
	real part is set to zero
	@param x FP8 instance to be initialised
	@param a FP4 to form imaginary part of FP8
 */
extern void FP8_YYY_from_FP4H(FP8_YYY *x,FP4_YYY *a);

/**	@brief Initialise FP8 from single FP
 *
	@param x FP8 instance to be initialised
	@param a FP to form real part of FP8
 */
extern void FP8_YYY_from_FP(FP8_YYY *x,FP_YYY *a);

/**	@brief Copy FP8 to another FP8
 *
	@param x FP8 instance, on exit = y
	@param y FP8 instance to be copied
 */
extern void FP8_YYY_copy(FP8_YYY *x,FP8_YYY *y);
/**	@brief Set FP8 to zero
 *
	@param x FP8 instance to be set to zero
 */
extern void FP8_YYY_zero(FP8_YYY *x);
/**	@brief Set FP8 to unity
 *
	@param x FP8 instance to be set to one
 */
extern void FP8_YYY_one(FP8_YYY *x);
/**	@brief Negation of FP8
 *
	@param x FP8 instance, on exit = -y
	@param y FP8 instance
 */
extern void FP8_YYY_neg(FP8_YYY *x,FP8_YYY *y);
/**	@brief Conjugation of FP8
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP8 instance, on exit = conj(y)
	@param y FP8 instance
 */
extern void FP8_YYY_conj(FP8_YYY *x,FP8_YYY *y);
/**	@brief Negative conjugation of FP8
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP8 instance, on exit = -conj(y)
	@param y FP8 instance
 */
extern void FP8_YYY_nconj(FP8_YYY *x,FP8_YYY *y);
/**	@brief addition of two FP8s
 *
	@param x FP8 instance, on exit = y+z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_YYY_add(FP8_YYY *x,FP8_YYY *y,FP8_YYY *z);
/**	@brief subtraction of two FP8s
 *
	@param x FP8 instance, on exit = y-z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_YYY_sub(FP8_YYY *x,FP8_YYY *y,FP8_YYY *z);
/**	@brief Multiplication of an FP8 by an FP4
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP4 multiplier
 */
extern void FP8_YYY_pmul(FP8_YYY *x,FP8_YYY *y,FP4_YYY *a);

/**	@brief Multiplication of an FP8 by an FP2
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP2 multiplier
 */
extern void FP8_YYY_qmul(FP8_YYY *x,FP8_YYY *y,FP2_YYY *a);

/**	@brief Multiplication of an FP8 by an FP
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP multiplier
 */
extern void FP8_YYY_tmul(FP8_YYY *x,FP8_YYY *y,FP_YYY *a);

/**	@brief Multiplication of an FP8 by a small integer
 *
	@param x FP8 instance, on exit = y*i
	@param y FP8 instance
	@param i an integer
 */
extern void FP8_YYY_imul(FP8_YYY *x,FP8_YYY *y,int i);
/**	@brief Squaring an FP8
 *
	@param x FP8 instance, on exit = y^2
	@param y FP8 instance
 */
extern void FP8_YYY_sqr(FP8_YYY *x,FP8_YYY *y);
/**	@brief Multiplication of two FP8s
 *
	@param x FP8 instance, on exit = y*z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_YYY_mul(FP8_YYY *x,FP8_YYY *y,FP8_YYY *z);
/**	@brief Inverting an FP8
 *
	@param x FP8 instance, on exit = 1/y
	@param y FP8 instance
 */
extern void FP8_YYY_inv(FP8_YYY *x,FP8_YYY *y);
/**	@brief Formats and outputs an FP8 to the console
 *
	@param x FP8 instance to be printed
 */
extern void FP8_YYY_output(FP8_YYY *x);
/**	@brief Formats and outputs an FP8 to the console in raw form (for debugging)
 *
	@param x FP8 instance to be printed
 */
extern void FP8_YYY_rawoutput(FP8_YYY *x);
/**	@brief multiplies an FP8 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP8 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP8_YYY_times_i(FP8_YYY *x);
/**	@brief multiplies an FP8 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP8 instance, on exit = (1+sqrt(-1)*x
 */
extern void FP8_YYY_times_i2(FP8_YYY *x);

/**	@brief Normalises the components of an FP8
 *
	@param x FP8 instance to be normalised
 */
extern void FP8_YYY_norm(FP8_YYY *x);
/**	@brief Reduces all components of possibly unreduced FP8 mod Modulus
 *
	@param x FP8 instance, on exit reduced mod Modulus
 */
extern void FP8_YYY_reduce(FP8_YYY *x);
/**	@brief Raises an FP8 to the power of a BIG
 *
	@param x FP8 instance, on exit = y^b
	@param y FP8 instance
	@param b BIG number
 */
extern void FP8_YYY_pow(FP8_YYY *x,FP8_YYY *y,BIG_XXX b);
/**	@brief Raises an FP8 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP8 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP8_YYY_frob(FP8_YYY *x,FP2_YYY *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP8 instance, on exit = w*x-conj(x)*y+z
	@param w FP8 instance
	@param x FP8 instance
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_YYY_xtr_A(FP8_YYY *r,FP8_YYY *w,FP8_YYY *x,FP8_YYY *y,FP8_YYY *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP8 instance, on exit = x^2-2*conj(x)
	@param x FP8 instance
 */
extern void FP8_YYY_xtr_D(FP8_YYY *r,FP8_YYY *x);
/**	@brief Calculates FP8 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP8 instance, on exit = trace(w^b)
	@param x FP8 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP8_YYY_xtr_pow(FP8_YYY *r,FP8_YYY *x,BIG_XXX b);
/**	@brief Calculates FP8 trace of c^a.d^b, where c and d are derived from FP8 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP8 instance, on exit = trace(c^a.d^b)
	@param c FP8 instance, trace of an FP12
	@param d FP8 instance, trace of an FP12
	@param e FP8 instance, trace of an FP12
	@param f FP8 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP8_YYY_xtr_pow2(FP8_YYY *r,FP8_YYY *c,FP8_YYY *d,FP8_YYY *e,FP8_YYY *f,BIG_XXX a,BIG_XXX b);


/**	@brief Calculate square root of an FP8
 *
	Square root
	@param r FP8 instance, on exit = sqrt(x)
	@param x FP8 instance
	@return 1 x is a QR, otherwise 0
 */
extern int  FP8_YYY_sqrt(FP8_YYY *r,FP8_YYY *x);


/**	@brief Conditional copy of FP8 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP8 instance, set to y if s!=0
	@param y another FP8 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP8_YYY_cmove(FP8_YYY *x,FP8_YYY *y,int s);


/**	@brief Divide FP8 number by QNR
 *
	Divide FP8 by the QNR
	@param x FP8 instance
 */
extern void FP8_YYY_div_i(FP8_YYY *x);

/**	@brief Divide FP8 number by QNR twice
 *
	Divide FP8 by the QNR twice
	@param x FP8 instance
 */
extern void FP8_YYY_div_i2(FP8_YYY *x);

/**	@brief Divide FP8 number by QNR/2
 *
	Divide FP8 by the QNR/2
	@param x FP8 instance
 */
extern void FP8_YYY_div_2i(FP8_YYY *x);


#endif

