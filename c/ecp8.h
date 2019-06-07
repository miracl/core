#ifndef ECP8_ZZZ_H
#define ECP8_ZZZ_H

#include "fp8_YYY.h"
#include "config_curve_ZZZ.h"


extern const BIG_XXX Fra_YYY; /**< real part of BN curve Frobenius Constant */
extern const BIG_XXX Frb_YYY; /**< imaginary part of BN curve Frobenius Constant */


/**
	@brief ECP8 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//    int inf; /**< Infinity Flag */
    FP8_YYY x;   /**< x-coordinate of point */
    FP8_YYY y;   /**< y-coordinate of point */
	FP8_YYY z;	/**< z-coordinate of point */
} ECP8_ZZZ;


/* Curve Params - see rom.c */
extern const int CURVE_A_ZZZ;		/**< Elliptic curve A parameter */
extern const int CURVE_B_I_ZZZ;		/**< Elliptic curve B parameter */
extern const BIG_XXX CURVE_B_ZZZ;     /**< Elliptic curve B parameter */
extern const BIG_XXX CURVE_Order_ZZZ; /**< Elliptic curve group order */
extern const BIG_XXX CURVE_Cof_ZZZ;   /**< Elliptic curve cofactor */
extern const BIG_XXX CURVE_Bnx_ZZZ;   /**< Elliptic curve parameter */


/* Generator point on G1 */
extern const BIG_XXX CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const BIG_XXX CURVE_Gy; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const BIG_XXX CURVE_Pxaaa_ZZZ; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxaab_ZZZ; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxaba_ZZZ; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxabb_ZZZ; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxbaa_ZZZ; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxbab_ZZZ; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxbba_ZZZ; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pxbbb_ZZZ; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const BIG_XXX CURVE_Pyaaa_ZZZ; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pyaab_ZZZ; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pyaba_ZZZ; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pyabb_ZZZ; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pybaa_ZZZ; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pybab_ZZZ; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pybba_ZZZ; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_XXX CURVE_Pybbb_ZZZ; /**< imaginary part of y-coordinate of generator point in group G2 */


/* ECP8 E(FP8) prototypes */
/**	@brief Tests for ECP8 point equal to infinity
 *
	@param P ECP8 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP8_ZZZ_isinf(ECP8_ZZZ *P);
/**	@brief Copy ECP8 point to another ECP8 point
 *
	@param P ECP8 instance, on exit = Q
	@param Q ECP8 instance to be copied
 */
extern void ECP8_ZZZ_copy(ECP8_ZZZ *P,ECP8_ZZZ *Q);
/**	@brief Set ECP8 to point-at-infinity
 *
	@param P ECP8 instance to be set to infinity
 */
extern void ECP8_ZZZ_inf(ECP8_ZZZ *P);
/**	@brief Tests for equality of two ECP8s
 *
	@param P ECP8 instance to be compared
	@param Q ECP8 instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP8_ZZZ_equals(ECP8_ZZZ *P,ECP8_ZZZ *Q);


/**	@brief Converts an ECP8 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP8 instance to be converted to affine form
 */
extern void ECP8_ZZZ_affine(ECP8_ZZZ *P);


/**	@brief Extract x and y coordinates of an ECP8 point P
 *
	If x=y, returns only x
	@param x FP8 on exit = x coordinate of point
	@param y FP8 on exit = y coordinate of point (unless x=y)
	@param P ECP8 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP8_ZZZ_get(FP8_YYY *x,FP8_YYY *y,ECP8_ZZZ *P);
/**	@brief Formats and outputs an ECP8 point to the console, converted to affine coordinates
 *
	@param P ECP8 instance to be printed
 */
extern void ECP8_ZZZ_output(ECP8_ZZZ *P);

/**	@brief Formats and outputs an ECP8 point to an octet string
 *
	The octet string is created in the form x|y.
	Convert the real and imaginary parts of the x and y coordinates to big-endian base 256 form.
	@param S output octet string
	@param P ECP8 instance to be converted to an octet string
 */
extern void ECP8_ZZZ_toOctet(octet *S,ECP8_ZZZ *P);
/**	@brief Creates an ECP8 point from an octet string
 *
	The octet string is in the form x|y
	The real and imaginary parts of the x and y coordinates are in big-endian base 256 form.
	@param P ECP8 instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP8_ZZZ_fromOctet(ECP8_ZZZ *P,octet *S);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP8 value of f(x)
	@param x FP8 instance
 */
extern void ECP8_ZZZ_rhs(FP8_YYY *r,FP8_YYY *x);
/**	@brief Set ECP8 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP8 instance to be set (x,y)
	@param x FP8 x coordinate of point
	@param y FP8 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP8_ZZZ_set(ECP8_ZZZ *P,FP8_YYY *x,FP8_YYY *y);
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Otherwise y coordinate is calculated from x.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP8_ZZZ_setx(ECP8_ZZZ *P,FP8_YYY *x);
/**	@brief Negation of an ECP8 point
 *
	@param P ECP8 instance, on exit = -P
 */
extern void ECP8_ZZZ_neg(ECP8_ZZZ *P);

/**	@brief Reduction of an ECP8 point
 *
	@param P ECP8 instance, on exit (x,y) are reduced wrt the modulus
 */
extern void ECP8_ZZZ_reduce(ECP8_ZZZ *P);


/**	@brief Doubles an ECP8 instance P and returns slope
 *
	@param P ECP8 instance, on exit =2*P
	@param lam FP8 instance, slope of line
 */
//extern int ECP8_ZZZ_sdbl(ECP8_ZZZ *P,FP8_YYY *lam);
/**	@brief Adds ECP8 instance Q to ECP8 instance P and returns slope
 *
	@param P ECP8 instance, on exit =P+Q
	@param Q ECP8 instance to be added to P
	@param lam FP8 instance, slope of line
 */
//extern int ECP8_ZZZ_sadd(ECP8_ZZZ *P,ECP8_ZZZ *Q,FP8_YYY *lam);


/**	@brief Doubles an ECP8 instance P
 *
	@param P ECP8 instance, on exit =2*P
 */
extern int ECP8_ZZZ_dbl(ECP8_ZZZ *P);
/**	@brief Adds ECP8 instance Q to ECP8 instance P
 *
	@param P ECP8 instance, on exit =P+Q
	@param Q ECP8 instance to be added to P
 */
extern int ECP8_ZZZ_add(ECP8_ZZZ *P,ECP8_ZZZ *Q);
/**	@brief Subtracts ECP instance Q from ECP8 instance P
 *
	@param P ECP8 instance, on exit =P-Q
	@param Q ECP8 instance to be subtracted from P
 */
extern void ECP8_ZZZ_sub(ECP8_ZZZ *P,ECP8_ZZZ *Q);
/**	@brief Multiplies an ECP8 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP8 instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP8_ZZZ_mul(ECP8_ZZZ *P,BIG_XXX b);

/**	@brief Calculates required Frobenius constants
 *
	Calculate Frobenius constants
	@param F array of FP2 precalculated constants

 */
extern void ECP8_ZZZ_frob_constants(FP2_YYY F[3]);

/**	@brief Multiplies an ECP8 instance P by the internal modulus p^n, using precalculated Frobenius constants
 *
	Fast point multiplication using Frobenius
	@param P ECP8 instance, on exit = p^n*P
	@param F array of FP2 precalculated Frobenius constant
	@param n power of prime

 */
extern void ECP8_ZZZ_frob(ECP8_ZZZ *P,FP2_YYY F[3],int n);

/**	@brief Calculates P=Sigma b[i]*Q[i] for i=0 to 7
 *
	@param P ECP8 instance, on exit = Sigma b[i]*Q[i] for i=0 to 7
	@param Q ECP8 array of 4 points
	@param b BIG array of 4 multipliers
 */
extern void ECP8_ZZZ_mul16(ECP8_ZZZ *P,ECP8_ZZZ *Q,BIG_XXX *b);


/**	@brief Maps random BIG to curve point of correct order
 *
	@param P ECP8 instance of correct order
	@param W OCTET byte array to be mapped
 */
extern void ECP8_ZZZ_mapit(ECP8_ZZZ *P,octet *w);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP8 instance
 */
extern void ECP8_ZZZ_generator(ECP8_ZZZ *G);


#endif