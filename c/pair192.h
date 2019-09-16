/*
   Copyright (C) 2019 MIRACL UK Ltd.

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.


    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

     https://www.gnu.org/licenses/agpl-3.0.en.html

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

   You can be released from the requirements of the license by purchasing
   a commercial license. Buying such a license is mandatory as soon as you
   develop commercial activities involving the MIRACL Core Crypto SDK
   without disclosing the source code of your own applications, or shipping
   the MIRACL Core Crypto SDK with a closed source product.
*/

/**
 * @file pair192.h
 * @author Mike Scott
 * @brief PAIR Header File
 *
 */

#ifndef PAIR192_ZZZ_H
#define PAIR192_ZZZ_H

#include "fp24_YYY.h"
#include "ecp4_ZZZ.h"
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
	@param T array of precomputed FP8 partial line functions
	@param GV a fixed ECP4 instance
 */
extern void PAIR_ZZZ_precomp(FP8_YYY T[], ECP4_ZZZ* GV);


/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP24 products of line functions
	@param PV ECP4 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_ZZZ_another(FP24_YYY r[], ECP4_ZZZ* PV, ECP_ZZZ* QV);

/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP24 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_ZZZ_another_pc(FP24_YYY r[], FP8_YYY T[], ECP_ZZZ *QV);


/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP24 result of the pairing calculation e(P,Q)
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_ZZZ_ate(FP24_YYY *r, ECP4_ZZZ *P, ECP_ZZZ *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP24 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP4 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_ZZZ_double_ate(FP24_YYY *r, ECP4_ZZZ *P, ECP_ZZZ *Q, ECP4_ZZZ *R, ECP_ZZZ *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP24, on exit = x^((p^12-1)/r)
 */
extern void PAIR_ZZZ_fexp(FP24_YYY *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_ZZZ_G1mul(ECP_ZZZ *Q, BIG_XXX b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP4 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_ZZZ_G2mul(ECP4_ZZZ *P, BIG_XXX b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP24 member of GT.
	@param b BIG exponent

 */
extern void PAIR_ZZZ_GTpow(FP24_YYY *x, BIG_XXX b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_ZZZ_G1member(ECP_ZZZ *P);

/**	@brief Tests ECP4 for membership of G2
 *
	@param P ECP4 member of G2
	@return true or false

 */
extern int PAIR_ZZZ_G2member(ECP4_ZZZ *P);
/**	@brief Tests FP24 for membership of GT
 *
	@param x FP24 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_ZZZ_GTmember(FP24_YYY *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_ZZZ_nbits(BIG_XXX n3, BIG_XXX n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP24 array, to be initialised to 1

 */
extern void PAIR_ZZZ_initmp(FP24_YYY r[]);


/**	@brief Miller loop
 *
 	@param res FP24 result
	@param r FP24 precomputed array of accumulated line functions

 */
extern void PAIR_ZZZ_miller(FP24_YYY *res, FP24_YYY r[]);


#endif