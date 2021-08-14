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

#ifndef PAIR4_ZZZ_H
#define PAIR4_ZZZ_H

#include "fp24_YYY.h"
#include "ecp4_ZZZ.h"
#include "ecp_ZZZ.h"

using namespace core;

namespace ZZZ {
/* Pairing constants */

extern const XXX::BIG CURVE_Bnx; /**< BN curve x parameter */
extern const XXX::BIG CURVE_Cru; /**< BN curve Cube Root of Unity */

extern const XXX::BIG CURVE_W[2];	 /**< BN curve constant for GLV decomposition */
extern const XXX::BIG CURVE_SB[2][2]; /**< BN curve constant for GLV decomposition */
extern const XXX::BIG CURVE_WB[4];	 /**< BN curve constant for GS decomposition */
extern const XXX::BIG CURVE_BB[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP8 partial line functions
	@param GV a fixed ECP4 instance
 */
extern void PAIR_precomp(YYY::FP8 T[], ECP4* GV);


/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP24 result of the pairing calculation e(P,Q)
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_ate(YYY::FP24 *r, ECP4 *P, ECP *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP24 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP4 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_double_ate(YYY::FP24 *r, ECP4 *P, ECP *Q, ECP4 *R, ECP *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP24, on exit = x^((p^12-1)/r)
 */
extern void PAIR_fexp(YYY::FP24 *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_G1mul(ECP *Q, XXX::BIG b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP4 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_G2mul(ECP4 *P, XXX::BIG b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP24 member of GT.
	@param b BIG exponent

 */
extern void PAIR_GTpow(YYY::FP24 *x, XXX::BIG b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_G1member(ZZZ::ECP *P);

/**	@brief Tests ECP4 for membership of G2
 *
	@param P ECP4 member of G2
	@return true or false

 */
extern int PAIR_G2member(ZZZ::ECP4 *P);


/**	@brief Tests FP24 for membership of cyclotomic subgroup
 *
	@param x FP24 instance
	@return 1 if x is cyclotomic, else return 0

 */
extern int PAIR_GTcyclotomic(YYY::FP24 *x);


/**	@brief Tests FP24 for full membership of GT
 *
	@param x FP24 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_GTmember(YYY::FP24 *x);


/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP24 products of line functions
	@param PV ECP4 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_another(YYY::FP24 r[], ECP4* PV, ECP* QV);

/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP24 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_another_pc(YYY::FP24 r[], YYY::FP8 T[], ECP *QV);


/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_nbits(XXX::BIG n3, XXX::BIG n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP24 array, to be initialised to 1

 */
extern void PAIR_initmp(YYY::FP24 r[]);


/**	@brief Miller loop
 *
 	@param res FP24 result
	@param r FP24 precomputed array of accumulated line functions

 */
extern void PAIR_miller(YYY::FP24 *res, YYY::FP24 r[]);

}

#endif
