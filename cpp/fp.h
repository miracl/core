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

#ifndef FP_YYY_H
#define FP_YYY_H

#include "big_XXX.h"
#include "config_field_YYY.h"

using namespace core;

#define MODBITS_YYY MBITS_YYY
#define TBITS_YYY (MBITS_YYY%BASEBITS_XXX)              /**< Number of active bits in top word */
#define TMASK_YYY (((chunk)1<<TBITS_YYY)-1)             /**< Mask for active bits in top word */
#define FEXCESS_YYY (((sign32)1<<MAXXES_YYY)-1)		    /**< 2^(BASEBITS*NLEN-MODBITS) - normalised BIG can be multiplied by less than this before reduction */
#define OMASK_YYY (-((chunk)(1)<<TBITS_YYY))            /**<  for masking out overflow bits */

namespace YYY {

/**
	@brief FP Structure - quadratic extension field
*/

typedef struct
{
    XXX::BIG g;	/**< Big representation of field element */
    sign32 XES;	/**< Excess */
} FP;

/* Field Params - see rom.c */
extern const XXX::BIG Modulus;	/**< Actual Modulus set in rom_field*.c */
extern const XXX::BIG ROI;	    /**< Root of Unity  set in rom_field*.c */
extern const XXX::BIG R2modp;	/**< Montgomery constant */
extern const chunk MConst;		/**< Constant associated with Modulus - for Montgomery = 1/p mod 2^BASEBITS */
extern const XXX::BIG SQRTm3;      /**< Square root of -3 */
extern const XXX::BIG CRu;    /**< Cube Root of Unity */
//extern const int BTset;			/**< Set Bit in Generalised Mersenne */
extern const XXX::BIG Fra; /**< real part of Pairing-friendly curve Frobenius Constant */
extern const XXX::BIG Frb; /**< imaginary part of Pairing-friendly curve Frobenius Constant */
extern const XXX::BIG TWK; /**< Tweak for square roots, pre-calculated from field norm */
//#define FUSED_MODMUL
//#define DEBUG_REDUCE

/* FP prototypes */

/**	@brief Create FP from integer
 *
	@param x FP to be initialised
	@param a integer
 */
extern void FP_from_int(FP *x,int a);


/**	@brief Tests for FP equal to zero mod Modulus
 *
	@param x FP number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP_iszilch(FP *x);

/**	@brief Tests for lexically largest 
 *
	@param x FP number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP_islarger(FP *x);

/**	@brief Serialize out FP  
 *
    @param b buffer for output
	@param x FP number to be serialized
 */
extern void FP_toBytes(char *b,FP *x);

/**	@brief Serialize in FP  
 *
	@param x FP number to be serialized
    @param b buffer for input
 */
extern void FP_fromBytes(FP *x,char *b);

/**	@brief Tests for FP equal to one mod Modulus
 *
	@param x FP number to be tested
	@return 1 if one, else returns 0
 */
extern int FP_isunity(FP *x);

/**	@brief Set FP to zero
 *
	@param x FP number to be set to 0
 */
extern void FP_zero(FP *x);

/**	@brief Copy an FP
 *
	@param y FP number to be copied to
	@param x FP to be copied from
 */
extern void FP_copy(FP *y, FP *x);

/**	@brief Copy from ROM to an FP
 *
	@param y FP number to be copied to
	@param x BIG to be copied from ROM
 */
extern void FP_rcopy(FP *y, const XXX::BIG x);


/**	@brief Compares two FPs
 *
	@param x FP number
	@param y FP number
	@return 1 if equal, else returns 0
 */
extern int FP_equals(FP *x, FP *y);


/**	@brief Conditional constant time swap of two FP numbers
 *
	Conditionally swaps parameters in constant time (without branching)
	@param x an FP number
	@param y another FP number
	@param s swap takes place if not equal to 0
 */
extern void FP_cswap(FP *x, FP *y, int s);
/**	@brief Conditional copy of FP number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x an FP number
	@param y another FP number
	@param s copy takes place if not equal to 0
 */
extern void FP_cmove(FP *x, FP *y, int s);
/**	@brief Converts from BIG integer to residue form mod Modulus
 *
	@param x BIG number to be converted
	@param y FP result
 */
extern void FP_nres(FP *y, XXX::BIG x);
/**	@brief Converts from residue form back to BIG integer form
 *
	@param y FP number to be converted to BIG
	@param x BIG result
 */
extern void FP_redc(XXX::BIG x, FP *y);
/**	@brief Sets FP to representation of unity in residue form
 *
	@param x FP number to be set equal to unity.
 */
extern void FP_one(FP *x);

/**	@brief returns "sign" of an FP
 *
	@param x FP number
    @return 0 for positive, 1 for negative
 */
extern int FP_sign(FP *x);


/**	@brief Reduces DBIG to BIG exploiting special form of the modulus
 *
	This function comes in different flavours depending on the form of Modulus that is currently in use.
	@param r BIG number, on exit = d mod Modulus
	@param d DBIG number to be reduced
 */
extern void FP_mod(XXX::BIG r, XXX::DBIG d);

#ifdef FUSED_MODMUL
extern void FP_modmul(XXX::BIG, XXX::BIG, XXX::BIG);
#endif

/**	@brief Fast Modular multiplication of two FPs, mod Modulus
 *
	Uses appropriate fast modular reduction method
	@param x FP number, on exit the modular product = y*z mod Modulus
	@param y FP number, the multiplicand
	@param z FP number, the multiplier
 */
extern void FP_mul(FP *x, FP *y, FP *z);
/**	@brief Fast Modular multiplication of an FP, by a small integer, mod Modulus
 *
	@param x FP number, on exit the modular product = y*i mod Modulus
	@param y FP number, the multiplicand
	@param i a small number, the multiplier
 */
extern void FP_imul(FP *x, FP *y, int i);
/**	@brief Fast Modular squaring of an FP, mod Modulus
 *
	Uses appropriate fast modular reduction method
	@param x FP number, on exit the modular product = y^2 mod Modulus
	@param y FP number, the number to be squared

 */
extern void FP_sqr(FP *x, FP *y);
/**	@brief Modular addition of two FPs, mod Modulus
 *
	@param x FP number, on exit the modular sum = y+z mod Modulus
	@param y FP number
	@param z FP number
 */
extern void FP_add(FP *x, FP *y, FP *z);
/**	@brief Modular subtraction of two FPs, mod Modulus
 *
	@param x FP number, on exit the modular difference = y-z mod Modulus
	@param y FP number
	@param z FP number
 */
extern void FP_sub(FP *x, FP *y, FP *z);
/**	@brief Modular division by 2 of an FP, mod Modulus
 *
	@param x FP number, on exit =y/2 mod Modulus
	@param y FP number
 */
extern void FP_div2(FP *x, FP *y);
/**	@brief Fast Modular exponentiation of an FP, to the power of a BIG, mod Modulus
 *
	@param x FP number, on exit  = y^z mod Modulus
	@param y FP number
	@param z BIG number exponent
 */
extern void FP_pow(FP *x, FP *y, XXX::BIG z);

/**	@brief Inverse square root precalculation
 *
	@param r FP number, on exit  = x^(p-2*e-1)/2^(e+1) mod Modulus
	@param x FP number
 */
extern void FP_progen(FP *r,FP *x);

/**	@brief Fast Modular square root of a an FP, mod Modulus
 *
	@param x FP number, on exit  = sqrt(y) mod Modulus
	@param y FP number, the number whose square root is calculated
    @param h an optional input precalculation
 */
extern void FP_sqrt(FP *x, FP *y, FP *h);

/**	@brief Modular negation of a an FP, mod Modulus
 *
	@param x FP number, on exit = -y mod Modulus
	@param y FP number

 */
extern void FP_neg(FP *x, FP *y);
/**	@brief Outputs an FP number to the console
 *
	Converts from residue form before output
	@param x an FP number
 */
extern void FP_output(FP *x);
/**	@brief Outputs an FP number to the console, in raw form
 *
	@param x a BIG number
 */
extern void FP_rawoutput(FP *x);
/**	@brief Reduces possibly unreduced FP mod Modulus
 *
	@param x FP number, on exit reduced mod Modulus
 */
extern void FP_reduce(FP *x);
/**	@brief normalizes FP
 *
	@param x FP number, on exit normalized
 */
extern void FP_norm(FP *x);
/**	@brief Tests for FP a quadratic residue mod Modulus
 *
	@param x FP number to be tested
    @param h an optional output precalculation
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_qr(FP *x,FP *h);


/**	@brief Simultaneous Inverse and Square root
 *
	@param i FP number, on exit = 1/x mod Modulus
	@param s FP number, on exit = sqrt(x) mod Modulus
	@param x FP number
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_invsqrt(FP *i,FP *s,FP *x);

/**	@brief Simultaneous Inverse and Square root of different numbers
 *
	@param i FP number, on exit = 1/i mod Modulus
	@param s FP number, on exit = sqrt(s) mod Modulus
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_tpo(FP* i, FP* s);


/**	@brief Modular inverse of a an FP, mod Modulus
 *
	@param x FP number, on exit = 1/y mod Modulus
	@param y FP number
    @param h an optional input precalculation
 */
extern void FP_inv(FP *x, FP *y, FP *h);

/**	@brief Special exponent of an FP, mod Modulus
 *
	@param x FP number, on exit = x^s mod Modulus
	@param y FP number
 */
extern void FP_fpow(FP *x, FP *y);

/**	@brief Generate random FP
 *
	@param x random FP number
	@param rng random number generator
 */
extern void FP_rand(FP *x, core::csprng *rng);

}

#endif
