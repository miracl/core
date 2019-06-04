#ifndef ECP_ZZZ_H
#define ECP_ZZZ_H

#include "fp_YYY.h"
#include "config_curve_ZZZ.h"

using namespace amcl;

namespace ZZZ {

/* Curve Params - see rom.c */
extern const int CURVE_A;     /**< Elliptic curve A parameter */
extern const int CURVE_B_I;
extern const int CURVE_Cof_I;
extern const XXX::BIG CURVE_B;     /**< Elliptic curve B parameter */
extern const XXX::BIG CURVE_Order; /**< Elliptic curve group order */
extern const XXX::BIG CURVE_Cof;   /**< Elliptic curve cofactor */

/* Generator point on G1 */
extern const XXX::BIG CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const XXX::BIG CURVE_Gy; /**< y-coordinate of generator point in group G1  */


/* For Pairings only */

/* Generator point on G2 */
extern const XXX::BIG CURVE_Pxa; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pya; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyb; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS24 curves ***/

extern const XXX::BIG CURVE_Pxaa; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxba; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyaa; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyba; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybb; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS48 curves ***/

extern const XXX::BIG CURVE_Pxaaa; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxaab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxaba; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxabb; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbaa; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbab; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbba; /**< real part of x-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pxbbb; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const XXX::BIG CURVE_Pyaaa; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyaab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyaba; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pyabb; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybaa; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybab; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybba; /**< real part of y-coordinate of generator point in group G2 */
extern const XXX::BIG CURVE_Pybbb; /**< imaginary part of y-coordinate of generator point in group G2 */



extern const XXX::BIG CURVE_Bnx; /**< BN curve x parameter */

extern const XXX::BIG CURVE_Cru; /**< BN curve Cube Root of Unity */

extern const XXX::BIG Fra; /**< real part of BN curve Frobenius Constant */
extern const XXX::BIG Frb; /**< imaginary part of BN curve Frobenius Constant */


extern const XXX::BIG CURVE_W[2];	 /**< BN curve constant for GLV decomposition */
extern const XXX::BIG CURVE_SB[2][2]; /**< BN curve constant for GLV decomposition */
extern const XXX::BIG CURVE_WB[4];	 /**< BN curve constant for GS decomposition */
extern const XXX::BIG CURVE_BB[4][4]; /**< BN curve constant for GS decomposition */


/**
	@brief ECP structure - Elliptic Curve Point over base field
*/

typedef struct
{
//    int inf; /**< Infinity Flag - not needed for Edwards representation */

    YYY::FP x; /**< x-coordinate of point */
#if CURVETYPE_ZZZ!=MONTGOMERY
    YYY::FP y; /**< y-coordinate of point. Not needed for Montgomery representation */
#endif
    YYY::FP z;/**< z-coordinate of point */
} ECP;


/* ECP E(Fp) prototypes */
/**	@brief Tests for ECP point equal to infinity
 *
	@param P ECP point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP_isinf(ECP *P);
/**	@brief Tests for equality of two ECPs
 *
	@param P ECP instance to be compared
	@param Q ECP instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP_equals(ECP *P,ECP *Q);
/**	@brief Copy ECP point to another ECP point
 *
	@param P ECP instance, on exit = Q
	@param Q ECP instance to be copied
 */
extern void ECP_copy(ECP *P,ECP *Q);
/**	@brief Negation of an ECP point
 *
	@param P ECP instance, on exit = -P
 */
extern void ECP_neg(ECP *P);
/**	@brief Set ECP to point-at-infinity
 *
	@param P ECP instance to be set to infinity
 */
extern void ECP_inf(ECP *P);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x) depends on form of elliptic curve, Weierstrass, Edwards or Montgomery.
	Used internally.
	@param r BIG n-residue value of f(x)
	@param x BIG n-residue x
 */
extern void ECP_rhs(YYY::FP *r,YYY::FP *x);
/**	@brief Set ECP to point(x,y) given just x and sign of y
 *
	Point P set to infinity if no such point on the curve. If x is on the curve then y is calculated from the curve equation.
	The correct y value (plus or minus) is selected given its sign s.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@param s an integer representing the "sign" of y, in fact its least significant bit.
 */
extern int ECP_setx(ECP *P,XXX::BIG x,int s);

#if CURVETYPE_ZZZ==MONTGOMERY
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Note that y coordinate is not needed.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_set(ECP *P,XXX::BIG x);
/**	@brief Extract x coordinate of an ECP point P
 *
	@param x BIG on exit = x coordinate of point
	@param P ECP instance (x,[y])
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP_get(XXX::BIG x,ECP *P);
/**	@brief Adds ECP instance Q to ECP instance P, given difference D=P-Q
 *
	Differential addition of points on a Montgomery curve
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
	@param D Difference between P and Q
 */
extern void ECP_add(ECP *P,ECP *Q,ECP *D);
#else
/**	@brief Set ECP to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP instance to be set (x,y)
	@param x BIG x coordinate of point
	@param y BIG y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_set(ECP *P,XXX::BIG x,XXX::BIG y);
/**	@brief Extract x and y coordinates of an ECP point P
 *
	If x=y, returns only x
	@param x BIG on exit = x coordinate of point
	@param y BIG on exit = y coordinate of point (unless x=y)
	@param P ECP instance (x,y)
	@return sign of y, or -1 if P is point-at-infinity
 */
extern int ECP_get(XXX::BIG x,XXX::BIG y,ECP *P);
/**	@brief Adds ECP instance Q to ECP instance P
 *
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
 */
extern void ECP_add(ECP *P,ECP *Q);
/**	@brief Subtracts ECP instance Q from ECP instance P
 *
	@param P ECP instance, on exit =P-Q
	@param Q ECP instance to be subtracted from P
 */
extern void ECP_sub(ECP *P,ECP *Q);
#endif
/**	@brief Converts an ECP point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP instance to be converted to affine form
 */
extern void ECP_affine(ECP *P);
/**	@brief Formats and outputs an ECP point to the console, in projective coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_outputxyz(ECP *P);
/**	@brief Formats and outputs an ECP point to the console, converted to affine coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_output(ECP * P);

/**	@brief Formats and outputs an ECP point to the console
 *
	@param P ECP instance to be printed
 */
extern void ECP_rawoutput(ECP * P);

/**	@brief Formats and outputs an ECP point to an octet string
 *
	The octet string is normally in the standard form 0x04|x|y
	Here x (and y) are the x and y coordinates in left justified big-endian base 256 form.
	For Montgomery curve it is 0x06|x
	If c is true, only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param c compression required, true or false
	@param S output octet string
	@param P ECP instance to be converted to an octet string
 */
extern void ECP_toOctet(octet *S,ECP *P,bool c);
/**	@brief Creates an ECP point from an octet string
 *
	The octet string is normally in the standard form 0x04|x|y
	Here x (and y) are the x and y coordinates in left justified big-endian base 256 form.
	For Montgomery curve it is 0x06|x
	If in compressed form only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param P ECP instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP_fromOctet(ECP *P,octet *S);
/**	@brief Doubles an ECP instance P
 *
	@param P ECP instance, on exit =2*P
 */
extern void ECP_dbl(ECP *P);
/**	@brief Multiplies an ECP instance P by a small integer, side-channel resistant
 *
	@param P ECP instance, on exit =i*P
	@param i small integer multiplier
	@param b maximum number of bits in multiplier
 */
extern void ECP_pinmul(ECP *P,int i,int b);
/**	@brief Multiplies an ECP instance P by a BIG, side-channel resistant
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP_mul(ECP *P,XXX::BIG b);
/**	@brief Calculates double multiplication P=e*P+f*Q, side-channel resistant
 *
	@param P ECP instance, on exit =e*P+f*Q
	@param Q ECP instance
	@param e BIG number multiplier
	@param f BIG number multiplier
 */
extern void ECP_mul2(ECP *P,ECP *Q,XXX::BIG e,XXX::BIG f);

/**	@brief Multiplies random point by co-factor
 *
	@param Q ECP multiplied by co-factor
 */
extern void ECP_cfp(ECP *Q);

/**	@brief Maps random BIG to curve point of correct order
 *
	@param Q ECP instance of correct order
	@param w OCTET byte array to be mapped
 */
extern void ECP_mapit(ECP *Q,octet *w);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP instance
 */
extern void ECP_generator(ECP *G);

}

#endif
