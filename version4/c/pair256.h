#ifndef PAIR256_ZZZ_H
#define PAIR256_ZZZ_H

#include "fp48_YYY.h"
#include "ecp8_ZZZ.h"
#include "ecp_ZZZ.h"


/* Pairing constants */

extern const BIG_XXX CURVE_Bnx_ZZZ; /**< BN curve x parameter */
extern const BIG_XXX CURVE_Cru_ZZZ; /**< BN curve Cube Root of Unity */

extern const BIG_XXX CURVE_W_ZZZ[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_XXX CURVE_SB_ZZZ[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_XXX CURVE_WB_ZZZ[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_XXX CURVE_BB_ZZZ[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP16 partial line functions
	@param GV a fixed ECP8 instance
 */
extern void PAIR_ZZZ_precomp(FP16_YYY T[],ECP8_ZZZ* GV);


/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP48 products of line functions
	@param PV ECP8 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_ZZZ_another(FP48_YYY r[],ECP8_ZZZ* PV,ECP_ZZZ* QV);


/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP48 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_ZZZ_another_pc(FP48_YYY r[],FP16_YYY T[],ECP_ZZZ *QV);

/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP48 result of the pairing calculation e(P,Q)
	@param P ECP8 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_ZZZ_ate(FP48_YYY *r,ECP8_ZZZ *P,ECP_ZZZ *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP48 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP8 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP8 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_ZZZ_double_ate(FP48_YYY *r,ECP8_ZZZ *P,ECP_ZZZ *Q,ECP8_ZZZ *R,ECP_ZZZ *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP48, on exit = x^((p^12-1)/r)
 */
extern void PAIR_ZZZ_fexp(FP48_YYY *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_ZZZ_G1mul(ECP_ZZZ *Q,BIG_XXX b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP8 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_ZZZ_G2mul(ECP8_ZZZ *P,BIG_XXX b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP48 member of GT.
	@param b BIG exponent

 */
extern void PAIR_ZZZ_GTpow(FP48_YYY *x,BIG_XXX b);
/**	@brief Tests FP48 for membership of GT
 *
	@param x FP48 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_ZZZ_GTmember(FP48_YYY *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_ZZZ_nbits(BIG_XXX n3,BIG_XXX n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP48 array, to be initialised to 1

 */
extern void PAIR_ZZZ_initmp(FP48_YYY r[]);


/**	@brief Miller loop
 *
 	@param res FP48 result
	@param r FP48 precomputed array of accumulated line functions

 */
extern void PAIR_ZZZ_miller(FP48_YYY *res,FP48_YYY r[]);
#endif
