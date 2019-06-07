#ifndef ECP4_ZZZ_H
#define ECP4_ZZZ_H

#include "fp4_YYY.h"
#include "config_curve_ZZZ.h"

using namespace amcl;

namespace YYY {

extern const XXX::BIG Fra; /**< real part of BN curve Frobenius Constant */
extern const XXX::BIG Frb; /**< imaginary part of BN curve Frobenius Constant */

}

namespace ZZZ {

/**
	@brief ECP4 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//    int inf; /**< Infinity Flag */
    YYY::FP4 x;   /**< x-coordinate of point */
    YYY::FP4 y;   /**< y-coordinate of point */
    YYY::FP4 z;   /**< z-coordinate of point */
} ECP4;


/* Curve Params - see rom.c */
extern const int CURVE_A;		/**< Elliptic curve A parameter */
extern const int CURVE_B_I;		/**< Elliptic curve B parameter */
extern const XXX::BIG CURVE_B;     /**< Elliptic curve B parameter */
extern const XXX::BIG CURVE_Order; /**< Elliptic curve group order */
extern const XXX::BIG CURVE_Cof;   /**< Elliptic curve cofactor */
extern const XXX::BIG CURVE_Bnx;   /**< Elliptic curve parameter */


/* Generator point on G1 */
extern const XXX::BIG CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const XXX::BIG CURVE_Gy; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const XXX::BIG CURVE_Pxaa; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxba; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyaa; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyba; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybb; /**< imaginary part of y-coordinate of generator point in group G2 */

/* ECP4 E(FP4) prototypes */
/**	@brief Tests for ECP4 point equal to infinity
 *
	@param P ECP4 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP4_isinf(ECP4 *P);
/**	@brief Copy ECP4 point to another ECP4 point
 *
	@param P ECP4 instance, on exit = Q
	@param Q ECP4 instance to be copied
 */
extern void ECP4_copy(ECP4 *P,ECP4 *Q);
/**	@brief Set ECP4 to point-at-infinity
 *
	@param P ECP4 instance to be set to infinity
 */
extern void ECP4_inf(ECP4 *P);
/**	@brief Tests for equality of two ECP4s
 *
	@param P ECP4 instance to be compared
	@param Q ECP4 instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP4_equals(ECP4 *P,ECP4 *Q);

/**	@brief Converts an ECP4 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP4 instance to be converted to affine form
 */
extern void ECP4_affine(ECP4 *P);


/**	@brief Extract x and y coordinates of an ECP4 point P
 *
	If x=y, returns only x
	@param x FP4 on exit = x coordinate of point
	@param y FP4 on exit = y coordinate of point (unless x=y)
	@param P ECP4 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP4_get(YYY::FP4 *x,YYY::FP4 *y,ECP4 *P);
/**	@brief Formats and outputs an ECP4 point to the console, converted to affine coordinates
 *
	@param P ECP4 instance to be printed
 */
extern void ECP4_output(ECP4 *P);

/**	@brief Formats and outputs an ECP4 point to an octet string
 *
	The octet string is created in the form x|y.
	Convert the real and imaginary parts of the x and y coordinates to big-endian base 256 form.
	@param S output octet string
	@param P ECP4 instance to be converted to an octet string
 */
extern void ECP4_toOctet(octet *S,ECP4 *P);
/**	@brief Creates an ECP4 point from an octet string
 *
	The octet string is in the form x|y
	The real and imaginary parts of the x and y coordinates are in big-endian base 256 form.
	@param P ECP4 instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP4_fromOctet(ECP4 *P,octet *S);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP4 value of f(x)
	@param x FP4 instance
 */
extern void ECP4_rhs(YYY::FP4 *r,YYY::FP4 *x);
/**	@brief Set ECP4 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP4 instance to be set (x,y)
	@param x FP4 x coordinate of point
	@param y FP4 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP4_set(ECP4 *P,YYY::FP4 *x,YYY::FP4 *y);
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Otherwise y coordinate is calculated from x.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP4_setx(ECP4 *P,YYY::FP4 *x);
/**	@brief Negation of an ECP4 point
 *
	@param P ECP4 instance, on exit = -P
 */
extern void ECP4_neg(ECP4 *P);

/**	@brief Reduction of an ECP4 point
 *
	@param P ECP4 instance, on exit (x,y) are reduced wrt the modulus
 */
extern void ECP4_reduce(ECP4 *P);


/**	@brief Doubles an ECP4 instance P and returns slope
 *
	@param P ECP4 instance, on exit =2*P
	@param lam FP4 instance, slope of line
 */
//extern int ECP4_sdbl(ECP4 *P,FP4 *lam);
/**	@brief Adds ECP4 instance Q to ECP4 instance P and returns slope
 *
	@param P ECP4 instance, on exit =P+Q
	@param Q ECP4 instance to be added to P
	@param lam FP4 instance, slope of line
 */
//extern int ECP4_sadd(ECP4 *P,ECP4 *Q,FP4 *lam);


/**	@brief Doubles an ECP4 instance P
 *
	@param P ECP4 instance, on exit =2*P
 */
extern int ECP4_dbl(ECP4 *P);
/**	@brief Adds ECP4 instance Q to ECP4 instance P
 *
	@param P ECP4 instance, on exit =P+Q
	@param Q ECP4 instance to be added to P
 */
extern int ECP4_add(ECP4 *P,ECP4 *Q);
/**	@brief Subtracts ECP instance Q from ECP4 instance P
 *
	@param P ECP4 instance, on exit =P-Q
	@param Q ECP4 instance to be subtracted from P
 */
extern void ECP4_sub(ECP4 *P,ECP4 *Q);
/**	@brief Multiplies an ECP4 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP4 instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP4_mul(ECP4 *P,XXX::BIG b);

/**	@brief Calculates required Frobenius constants
 *
	Calculate Frobenius constants
	@param F array of FP2 precalculated constants

 */
extern void ECP4_frob_constants(YYY::FP2 F[3]);

/**	@brief Multiplies an ECP4 instance P by the internal modulus p^n, using precalculated Frobenius constants
 *
	Fast point multiplication using Frobenius
	@param P ECP4 instance, on exit = p^n*P
	@param F array of FP2 precalculated Frobenius constant
	@param n power of prime

 */
extern void ECP4_frob(ECP4 *P,YYY::FP2 F[3],int n);

/**	@brief Calculates P=Sigma b[i]*Q[i] for i=0 to 7
 *
	@param P ECP4 instance, on exit = Sigma b[i]*Q[i] for i=0 to 7
	@param Q ECP4 array of 4 points
	@param b BIG array of 4 multipliers
 */
extern void ECP4_mul8(ECP4 *P,ECP4 *Q,XXX::BIG *b);


/**	@brief Maps random BIG to curve point of correct order
 *
	@param P ECP4 instance of correct order
	@param W OCTET byte array to be mapped
 */
extern void ECP4_mapit(ECP4 *P,octet *w);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP4 instance
 */
extern void ECP4_generator(ECP4 *G);


}

#endif