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

/* Test and benchmark elliptic curve and RSA functions
    First build core.a from config*.py file
    gcc -O3 benchtest_all.c core.a -o benchtest_all
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rsa_2048.h"
#include "ecp_Ed25519.h"
#include "pair_BN254.h"


#if CHUNK==32 || CHUNK==64
#include "ecp_NIST256.h"
#include "ecp_Ed448.h"
#include "pair_BLS12383.h"
#include "pair4_BLS24479.h"
#include "pair8_BLS48556.h"
#endif

#define MIN_TIME 10.0
#define MIN_ITERS 10

/* IMPORTANT - this must be edited if curve is changed */

#if CHUNK==16

#define BIG_ED BIG_256_13
#define BIG_ED_rcopy BIG_256_13_rcopy
#define BIG_ED_randomnum BIG_256_13_randomnum
#define BIG_ED_randtrunc BIG_256_13_randtrunc

#define BIG_BN BIG_256_13
#define BIG_BN_rcopy BIG_256_13_rcopy
#define BIG_BN_randomnum BIG_256_13_randomnum
#define BIG_BN_randtrunc BIG_256_13_randtrunc

#endif

#if CHUNK==32

#define BIG_ED BIG_256_29
#define BIG_ED_rcopy BIG_256_29_rcopy
#define BIG_ED_randomnum BIG_256_29_randomnum
#define BIG_ED_randtrunc BIG_256_29_randtrunc

#define BIG_NT BIG_256_28
#define BIG_NT_rcopy BIG_256_28_rcopy
#define BIG_NT_randomnum BIG_256_28_randomnum
#define BIG_NT_randtrunc BIG_256_28_randtrunc

#define BIG_GL BIG_448_29
#define BIG_GL_rcopy BIG_448_29_rcopy
#define BIG_GL_randomnum BIG_448_29_randomnum
#define BIG_GL_randtrunc BIG_448_29_randtrunc

#define BIG_BN BIG_256_28
#define BIG_BN_rcopy BIG_256_28_rcopy
#define BIG_BN_randomnum BIG_256_28_randomnum
#define BIG_BN_randtrunc BIG_256_28_randtrunc

#define BIG_BLS12 BIG_384_29   // 384
#define BIG_BLS12_rcopy BIG_384_29_rcopy
#define BIG_BLS12_randomnum BIG_384_29_randomnum
#define BIG_BLS12_randtrunc BIG_384_29_randtrunc

#define BIG_BLS24479 BIG_480_29
#define BIG_BLS24479_rcopy BIG_480_29_rcopy
#define BIG_BLS24479_randomnum BIG_480_29_randomnum
#define BIG_BLS24479_randtrunc BIG_480_29_randtrunc

#define BIG_BLS48556 BIG_560_29
#define BIG_BLS48556_rcopy BIG_560_29_rcopy
#define BIG_BLS48556_randomnum BIG_560_29_randomnum
#define BIG_BLS48556_randtrunc BIG_560_29_randtrunc

#endif

#if CHUNK==64

#define BIG_ED BIG_256_56
#define BIG_ED_rcopy BIG_256_56_rcopy
#define BIG_ED_randomnum BIG_256_56_randomnum
#define BIG_ED_randtrunc BIG_256_56_randtrunc

#define BIG_NT BIG_256_56
#define BIG_NT_rcopy BIG_256_56_rcopy
#define BIG_NT_randomnum BIG_256_56_randomnum
#define BIG_NT_randtrunc BIG_256_56_randtrunc

#define BIG_GL BIG_448_58
#define BIG_GL_rcopy BIG_448_58_rcopy
#define BIG_GL_randomnum BIG_448_58_randomnum
#define BIG_GL_randtrunc BIG_448_58_randtrunc

#define BIG_BN BIG_256_56
#define BIG_BN_rcopy BIG_256_56_rcopy
#define BIG_BN_randomnum BIG_256_56_randomnum
#define BIG_BN_randtrunc BIG_256_56_randtrunc

#define BIG_BLS12 BIG_384_58
#define BIG_BLS12_rcopy BIG_384_58_rcopy
#define BIG_BLS12_randomnum BIG_384_58_randomnum
#define BIG_BLS12_randtrunc BIG_384_58_randtrunc

#define BIG_BLS24479 BIG_480_56
#define BIG_BLS24479_rcopy BIG_480_56_rcopy
#define BIG_BLS24479_randomnum BIG_480_56_randomnum
#define BIG_BLS24479_randtrunc BIG_480_56_randtrunc

#define BIG_BLS48556 BIG_560_58
#define BIG_BLS48556_rcopy BIG_560_58_rcopy
#define BIG_BLS48556_randomnum BIG_560_58_randomnum
#define BIG_BLS48556_randtrunc BIG_560_58_randtrunc

#endif

int ed25519(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;
    ECP_Ed25519 EP, EG;
    BIG_ED s, r, x, y;
    FP_F25519 rw;
    printf("\nTesting/Timing Ed25519 ECC\n");

#if CURVETYPE_Ed25519==WEIERSTRASS
    printf("Weierstrass parameterization\n");
#endif
#if CURVETYPE_Ed25519==EDWARDS
    printf("Edwards parameterization\n");
#endif
#if CURVETYPE_Ed25519==MONTGOMERY
    printf("Montgomery parameterization\n");
#endif

#if MODTYPE_F25519 == PSEUDO_MERSENNE
    printf("Pseudo-Mersenne Modulus\n");
#endif

#if MODTYPE_F25519 == GENERALISED_MERSENNE
    printf("Generalised-Mersenne Modulus\n");
#endif

#if MODTYPE_F25519 == MONTGOMERY_FRIENDLY
    printf("Montgomery Friendly Modulus\n");
#endif

#if MODTYPE_F25519 == NOT_SPECIAL
    printf("Not special Modulus\n");
#endif


#if CHUNK==16
    printf("16-bit Build\n");
#endif
#if CHUNK==32
    printf("32-bit Build\n");
#endif
#if CHUNK==64
    printf("64-bit Build\n");
#endif

    ECP_Ed25519_generator(&EG);

    FP_F25519_rand(&rw,RNG);
    ECP_Ed25519_map2point(&EP,&rw);
    ECP_Ed25519_cfp(&EP);

    if (ECP_Ed25519_isinf(&EP))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }    

    BIG_ED_rcopy(r, CURVE_Order_Ed25519);
    BIG_ED_randtrunc(s, r, 2 * CURVE_SECURITY_Ed25519, RNG);
    ECP_Ed25519_copy(&EP, &EG);
    ECP_Ed25519_mul(&EP, r);

    if (!ECP_Ed25519_isinf(&EP))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_Ed25519_copy(&EP, &EG);
        ECP_Ed25519_mul(&EP, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("EC  mul - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    return 0;
}

#if CHUNK==32 || CHUNK==64

int nist256(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;
    ECP_NIST256 EP, EG;
    FP_NIST256 rw;
    BIG_NT s, r, x, y;
    printf("\nTesting/Timing NIST256 ECC\n");

#if CURVETYPE_NIST256==WEIERSTRASS
    printf("Weierstrass parameterization\n");
#endif
#if CURVETYPE_NIST256==EDWARDS
    printf("Edwards parameterization\n");
#endif
#if CURVETYPE_NIST256==MONTGOMERY
    printf("Montgomery parameterization\n");
#endif

#if MODTYPE_NIST256 == PSEUDO_MERSENNE
    printf("Pseudo-Mersenne Modulus\n");
#endif

#if MODTYPE_NIST256 == GENERALISED_MERSENNE
    printf("Generalised-Mersenne Modulus\n");
#endif

#if MODTYPE_NIST256 == MONTGOMERY_FRIENDLY
    printf("Montgomery Friendly Modulus\n");
#endif

#if MODTYPE_NIST256 == NOT_SPECIAL
    printf("Not special Modulus\n");
#endif

#if CHUNK==16
    printf("16-bit Build\n");
#endif
#if CHUNK==32
    printf("32-bit Build\n");
#endif
#if CHUNK==64
    printf("64-bit Build\n");
#endif

    ECP_NIST256_generator(&EG);

    FP_NIST256_rand(&rw,RNG);
    ECP_NIST256_map2point(&EP,&rw);
    ECP_NIST256_cfp(&EP);

    if (ECP_NIST256_isinf(&EP))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }    

    BIG_NT_rcopy(r, CURVE_Order_NIST256);
    BIG_NT_randtrunc(s, r, 2 * CURVE_SECURITY_NIST256, RNG);
    ECP_NIST256_copy(&EP, &EG);
    ECP_NIST256_mul(&EP, r);

    if (!ECP_NIST256_isinf(&EP))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_NIST256_copy(&EP, &EG);
        ECP_NIST256_mul(&EP, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("EC  mul - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    return 0;
}

int ed448(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;
    ECP_Ed448 EP, EG;
    BIG_GL s, r, x, y;
    FP_F448 rw;
    printf("\nTesting/Timing Ed448 ECC\n");

#if CURVETYPE_Ed448==WEIERSTRASS
    printf("Weierstrass parameterization\n");
#endif
#if CURVETYPE_Ed448==EDWARDS
    printf("Edwards parameterization\n");
#endif
#if CURVETYPE_Ed448==MONTGOMERY
    printf("Montgomery parameterization\n");
#endif

#if MODTYPE_F448 == PSEUDO_MERSENNE
    printf("Pseudo-Mersenne Modulus\n");
#endif

#if MODTYPE_F448 == GENERALISED_MERSENNE
    printf("Generalised-Mersenne Modulus\n");
#endif

#if MODTYPE_F448 == MONTGOMERY_FRIENDLY
    printf("Montgomery Friendly Modulus\n");
#endif

#if CHUNK==16
    printf("16-bit Build\n");
#endif
#if CHUNK==32
    printf("32-bit Build\n");
#endif
#if CHUNK==64
    printf("64-bit Build\n");
#endif

    ECP_Ed448_generator(&EG);

    FP_F448_rand(&rw,RNG);
    ECP_Ed448_map2point(&EP,&rw);
    ECP_Ed448_cfp(&EP);

    if (ECP_Ed448_isinf(&EP))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }   

    BIG_GL_rcopy(r, CURVE_Order_Ed448);
    BIG_GL_randtrunc(s, r, 2 * CURVE_SECURITY_Ed448, RNG);
    ECP_Ed448_copy(&EP, &EG);
    ECP_Ed448_mul(&EP, r);

    if (!ECP_Ed448_isinf(&EP))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_Ed448_copy(&EP, &EG);
        ECP_Ed448_mul(&EP, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("EC  mul - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    return 0;
}
#endif

int bn254(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;

    ECP_BN254 P, G;
    ECP2_BN254 Q, W;
    FP12_BN254 g, w;
    FP4_BN254 cm;
    FP2_BN254 rz2;
    FP_BN254 rz;

    BIG_BN s, r, x, y;
    printf("\nTesting/Timing BN254 Pairings\n");

    ECP_BN254_generator(&G);

    FP_BN254_rand(&rz,RNG);
    ECP_BN254_map2point(&P,&rz);
    ECP_BN254_cfp(&P);

    if (ECP_BN254_isinf(&P))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }   

    BIG_BN_rcopy(r, CURVE_Order_BN254);
    BIG_BN_randtrunc(s, r, 2 * CURVE_SECURITY_BN254, RNG);
    ECP_BN254_copy(&P, &G);
    PAIR_BN254_G1mul(&P, r);

    if (!ECP_BN254_isinf(&P))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_BN254_copy(&P, &G);
        PAIR_BN254_G1mul(&P, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G1 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP2_BN254_generator(&W);

    FP2_BN254_rand(&rz2,RNG);
    ECP2_BN254_map2point(&Q,&rz2);
    ECP2_BN254_cfp(&Q);

    if (ECP2_BN254_isinf(&Q))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }
    ECP2_BN254_mul(&Q,r);
    if (!ECP2_BN254_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    ECP2_BN254_copy(&Q, &W);
    ECP2_BN254_mul(&Q, r);

    if (!ECP2_BN254_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP2_BN254_copy(&Q, &W);
        PAIR_BN254_G2mul(&Q, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G2 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    PAIR_BN254_ate(&w, &Q, &P);
    PAIR_BN254_fexp(&w);

    FP12_BN254_copy(&g, &w);

    PAIR_BN254_GTpow(&g, r);

    if (!FP12_BN254_isunity(&g))
    {
        printf("FAILURE - g^r!=1\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        FP12_BN254_copy(&g, &w);
        PAIR_BN254_GTpow(&g, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    FP12_BN254_copy(&g, &w);

    iterations = 0;
    start = clock();
    do {
        FP12_BN254_compow(&cm, &g, s, r);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow (compressed) - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        PAIR_BN254_ate(&w, &Q, &P);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing ATE         - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        FP12_BN254_copy(&g, &w);
        PAIR_BN254_fexp(&g);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing FEXP        - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP_BN254_copy(&P, &G);
    ECP2_BN254_copy(&Q, &W);

    PAIR_BN254_G1mul(&P, s);
    PAIR_BN254_ate(&g, &Q, &P);
    PAIR_BN254_fexp(&g);

    ECP_BN254_copy(&P, &G);

    PAIR_BN254_G2mul(&Q, s);
    PAIR_BN254_ate(&w, &Q, &P);
    PAIR_BN254_fexp(&w);

	if (!PAIR_BN254_G1member(&P))
	{
        printf("FAILURE - P not in G1\n");
        return 0;
	}

	if (!PAIR_BN254_G2member(&Q))
	{
        printf("FAILURE - Q not in G2\n");
        return 0;
	}

	if (!PAIR_BN254_GTmember(&w))
	{
        printf("FAILURE - e(Q,P) not in GT\n");
        return 0;
	}


    if (!FP12_BN254_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
        return 0;
    }

    ECP2_BN254_copy(&Q, &W);
    PAIR_BN254_ate(&g, &Q, &P);
    PAIR_BN254_fexp(&g);

    PAIR_BN254_GTpow(&g, s);

    if (!FP12_BN254_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
        return 0;
    }
    return 0;
}

#if CHUNK==32 || CHUNK==64

int bls383(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;

    ECP_BLS12383 P, G;
    ECP2_BLS12383 Q, W;
    FP12_BLS12383 g, w;
    FP4_BLS12383 cm;
    FP2_BLS12383 rz2;
    FP_BLS12383 rz;

    BIG_BLS12 s, r, x, y;
    printf("\nTesting/Timing BLS12383 Pairings\n");

    ECP_BLS12383_generator(&G);

    FP_BLS12383_rand(&rz,RNG);
    ECP_BLS12383_map2point(&P,&rz);
    ECP_BLS12383_cfp(&P);

    if (ECP_BLS12383_isinf(&P))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }   

    BIG_BLS12_rcopy(r, CURVE_Order_BLS12383);
    BIG_BLS12_randtrunc(s, r, 2 * CURVE_SECURITY_BLS12383, RNG);
    ECP_BLS12383_copy(&P, &G);
    PAIR_BLS12383_G1mul(&P, r);

    if (!ECP_BLS12383_isinf(&P))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_BLS12383_copy(&P, &G);
        PAIR_BLS12383_G1mul(&P, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G1 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP2_BLS12383_generator(&W);

    FP2_BLS12383_rand(&rz2,RNG);
    ECP2_BLS12383_map2point(&Q,&rz2);
    ECP2_BLS12383_cfp(&Q);

    if (ECP2_BLS12383_isinf(&Q))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }
    ECP2_BLS12383_mul(&Q,r);
    if (!ECP2_BLS12383_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    ECP2_BLS12383_copy(&Q, &W);
    ECP2_BLS12383_mul(&Q, r);

    if (!ECP2_BLS12383_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP2_BLS12383_copy(&Q, &W);
        PAIR_BLS12383_G2mul(&Q, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G2 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    PAIR_BLS12383_ate(&w, &Q, &P);
    PAIR_BLS12383_fexp(&w);

    FP12_BLS12383_copy(&g, &w);

    PAIR_BLS12383_GTpow(&g, r);

    if (!FP12_BLS12383_isunity(&g))
    {
        printf("FAILURE - g^r!=1\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        FP12_BLS12383_copy(&g, &w);
        PAIR_BLS12383_GTpow(&g, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    FP12_BLS12383_copy(&g, &w);

    iterations = 0;
    start = clock();
    do {
        FP12_BLS12383_compow(&cm, &g, s, r);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow (compressed) - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        PAIR_BLS12383_ate(&w, &Q, &P);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing ATE         - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        FP12_BLS12383_copy(&g, &w);
        PAIR_BLS12383_fexp(&g);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing FEXP        - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);
    
    ECP_BLS12383_copy(&P, &G);
    ECP2_BLS12383_copy(&Q, &W);

    PAIR_BLS12383_G1mul(&P, s);
    PAIR_BLS12383_ate(&g, &Q, &P);
    PAIR_BLS12383_fexp(&g);

    ECP_BLS12383_copy(&P, &G);

    PAIR_BLS12383_G2mul(&Q, s);
    PAIR_BLS12383_ate(&w, &Q, &P);
    PAIR_BLS12383_fexp(&w);

	if (!PAIR_BLS12383_G1member(&P))
	{
        printf("FAILURE - P not in G1\n");
        return 0;
	}

	if (!PAIR_BLS12383_G2member(&Q))
	{
        printf("FAILURE - Q not in G2\n");
        return 0;
	}

	if (!PAIR_BLS12383_GTmember(&w))
	{
        printf("FAILURE - e(Q,P) not in GT\n");
        return 0;
	}

    if (!FP12_BLS12383_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
        return 0;
    }

    ECP2_BLS12383_copy(&Q, &W);
    PAIR_BLS12383_ate(&g, &Q, &P);
    PAIR_BLS12383_fexp(&g);

    PAIR_BLS12383_GTpow(&g, s);

    if (!FP12_BLS12383_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
        return 0;
    }
    return 0;
}

int bls24(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;

    ECP_BLS24479 P, G;
    ECP4_BLS24479 Q, W;
    FP24_BLS24479 g, w;
    FP8_BLS24479 cm;
    FP4_BLS24479 rz4;
    FP_BLS24479 rz;

    BIG_BLS24479 s, r, x, y;

    printf("\nTesting/Timing BLS24479 Pairings\n");

    ECP_BLS24479_generator(&G);

    FP_BLS24479_rand(&rz,RNG);
    ECP_BLS24479_map2point(&P,&rz);
    ECP_BLS24479_cfp(&P);

    if (ECP_BLS24479_isinf(&P))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }   

    BIG_BLS24479_rcopy(r, CURVE_Order_BLS24479);
    BIG_BLS24479_randtrunc(s, r, 2 * CURVE_SECURITY_BLS24479, RNG);
    ECP_BLS24479_copy(&P, &G);
    PAIR_BLS24479_G1mul(&P, r);

    if (!ECP_BLS24479_isinf(&P))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_BLS24479_copy(&P, &G);
        PAIR_BLS24479_G1mul(&P, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G1 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP4_BLS24479_generator(&W);

    FP4_BLS24479_rand(&rz4,RNG);
    ECP4_BLS24479_map2point(&Q,&rz4);
    ECP4_BLS24479_cfp(&Q);

    if (ECP4_BLS24479_isinf(&Q))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }
    ECP4_BLS24479_mul(&Q,r);
    if (!ECP4_BLS24479_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    ECP4_BLS24479_copy(&Q, &W);
    ECP4_BLS24479_mul(&Q, r);

    if (!ECP4_BLS24479_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP4_BLS24479_copy(&Q, &W);
        PAIR_BLS24479_G2mul(&Q, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G2 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    PAIR_BLS24479_ate(&w, &Q, &P);
    PAIR_BLS24479_fexp(&w);

    FP24_BLS24479_copy(&g, &w);

    PAIR_BLS24479_GTpow(&g, r);

    if (!FP24_BLS24479_isunity(&g))
    {
        printf("FAILURE - g^r!=1\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        FP24_BLS24479_copy(&g, &w);
        PAIR_BLS24479_GTpow(&g, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        PAIR_BLS24479_ate(&w, &Q, &P);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing ATE         - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        FP24_BLS24479_copy(&g, &w);
        PAIR_BLS24479_fexp(&g);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing FEXP        - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP_BLS24479_copy(&P, &G);
    ECP4_BLS24479_copy(&Q, &W);

    PAIR_BLS24479_G1mul(&P, s);
    PAIR_BLS24479_ate(&g, &Q, &P);
    PAIR_BLS24479_fexp(&g);

    ECP_BLS24479_copy(&P, &G);

    PAIR_BLS24479_G2mul(&Q, s);
    PAIR_BLS24479_ate(&w, &Q, &P);
    PAIR_BLS24479_fexp(&w);

	if (!PAIR_BLS24479_G1member(&P))
	{
        printf("FAILURE - P not in G1\n");
        return 0;
	}

	if (!PAIR_BLS24479_G2member(&Q))
	{
        printf("FAILURE - Q not in G2\n");
        return 0;
	}

	if (!PAIR_BLS24479_GTmember(&w))
	{
        printf("FAILURE - e(Q,P) not in GT\n");
        return 0;
	}

    if (!FP24_BLS24479_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
        return 0;
    }

    ECP4_BLS24479_copy(&Q, &W);
    PAIR_BLS24479_ate(&g, &Q, &P);
    PAIR_BLS24479_fexp(&g);

    PAIR_BLS24479_GTpow(&g, s);

    if (!FP24_BLS24479_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
        return 0;
    }
    return 0;
}


int bls48(csprng *RNG)
{
    int i, iterations;
    clock_t start;
    double elapsed;

    ECP_BLS48556 P, G;
    ECP8_BLS48556 Q, W;
    FP48_BLS48556 g, w;
    FP16_BLS48556 cm;
    FP8_BLS48556 rz8;
    FP_BLS48556 rz;


    BIG_BLS48556 s, r, x, y;
    printf("\nTesting/Timing BLS48556 Pairings\n");

    ECP_BLS48556_generator(&G);

    FP_BLS48556_rand(&rz,RNG);
    ECP_BLS48556_map2point(&P,&rz);
    ECP_BLS48556_cfp(&P);

    if (ECP_BLS48556_isinf(&P))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }  

    BIG_BLS48556_rcopy(r, CURVE_Order_BLS48556);
    BIG_BLS48556_randtrunc(s, r, 2 * CURVE_SECURITY_BLS48556, RNG);
    ECP_BLS48556_copy(&P, &G);
    PAIR_BLS48556_G1mul(&P, r);

    if (!ECP_BLS48556_isinf(&P))
    {
        printf("FAILURE - rG!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP_BLS48556_copy(&P, &G);
        PAIR_BLS48556_G1mul(&P, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G1 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP8_BLS48556_generator(&W);

    FP8_BLS48556_rand(&rz8,RNG);
    ECP8_BLS48556_map2point(&Q,&rz8);
    ECP8_BLS48556_cfp(&Q);

    if (ECP8_BLS48556_isinf(&Q))
    {
        printf("HASHING FAILURE - P=O\n");
        return 0;
    }
    ECP8_BLS48556_mul(&Q,r);
    if (!ECP8_BLS48556_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    ECP8_BLS48556_copy(&Q, &W);
    ECP8_BLS48556_mul(&Q, r);

    if (!ECP8_BLS48556_isinf(&Q))
    {
        printf("FAILURE - rQ!=O\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        ECP8_BLS48556_copy(&Q, &W);
        PAIR_BLS48556_G2mul(&Q, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("G2 mul              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    PAIR_BLS48556_ate(&w, &Q, &P);
    PAIR_BLS48556_fexp(&w);

    FP48_BLS48556_copy(&g, &w);

    PAIR_BLS48556_GTpow(&g, r);

    if (!FP48_BLS48556_isunity(&g))
    {
        printf("FAILURE - g^r!=1\n");
        return 0;
    }

    iterations = 0;
    start = clock();
    do {
        FP48_BLS48556_copy(&g, &w);
        PAIR_BLS48556_GTpow(&g, s);

        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("GT pow              - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        PAIR_BLS48556_ate(&w, &Q, &P);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing ATE         - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        FP48_BLS48556_copy(&g, &w);
        PAIR_BLS48556_fexp(&g);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("PAIRing FEXP        - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    ECP_BLS48556_copy(&P, &G);
    ECP8_BLS48556_copy(&Q, &W);

    PAIR_BLS48556_G1mul(&P, s);
    PAIR_BLS48556_ate(&g, &Q, &P);
    PAIR_BLS48556_fexp(&g);

    ECP_BLS48556_copy(&P, &G);

    PAIR_BLS48556_G2mul(&Q, s);
    PAIR_BLS48556_ate(&w, &Q, &P);
    PAIR_BLS48556_fexp(&w);

	if (!PAIR_BLS48556_G1member(&P))
	{
        printf("FAILURE - P not in G1\n");
        return 0;
	}

	if (!PAIR_BLS48556_G2member(&Q))
	{
        printf("FAILURE - Q not in G2\n");
        return 0;
	}

	if (!PAIR_BLS48556_GTmember(&w))
	{
        printf("FAILURE - e(Q,P) not in GT\n");
        return 0;
	}

    if (!FP48_BLS48556_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
        return 0;
    }

    ECP8_BLS48556_copy(&Q, &W);
    PAIR_BLS48556_ate(&g, &Q, &P);
    PAIR_BLS48556_fexp(&g);

    PAIR_BLS48556_GTpow(&g, s);

    if (!FP48_BLS48556_equals(&g, &w))
    {
        printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
        return 0;
    }
    return 0;
}
#endif

int rsa2048(csprng *RNG)
{
    rsa_public_key_2048 pub;
    rsa_private_key_2048 priv;

    int i, iterations;
    clock_t start;
    double elapsed;

    char m[RFS_2048], d[RFS_2048], c[RFS_2048];
    octet M = {0, sizeof(m), m};
    octet D = {0, sizeof(d), d};
    octet C = {0, sizeof(c), c};

    printf("\nTesting/Timing 2048-bit RSA\n");

    printf("Generating 2048-bit RSA public/private key pair\n");

    iterations = 0;
    start = clock();
    do {
        RSA_2048_KEY_PAIR(RNG, 65537, &priv, &pub, NULL, NULL);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("RSA gen - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    M.len = RFS_2048;
    for (i = 0; i < RFS_2048; i++) M.val[i] = i % 128;

    iterations = 0;
    start = clock();
    do {
        RSA_2048_ENCRYPT(&pub, &M, &C);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("RSA enc - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    iterations = 0;
    start = clock();
    do {
        RSA_2048_DECRYPT(&priv, &C, &D);
        iterations++;
        elapsed = (clock() - start) / (double)CLOCKS_PER_SEC;
    } while (elapsed < MIN_TIME || iterations < MIN_ITERS);
    elapsed = 1000.0 * elapsed / iterations;
    printf("RSA dec - %8d iterations  ", iterations);
    printf(" %8.2lf ms per iteration\n", elapsed);

    for (i = 0; i < RFS_2048; i++)
    {
        if (M.val[i] != D.val[i])
        {
            printf("FAILURE - RSA decryption\n");
            return 0;
        }
    }

    printf("All tests pass\n");

    return 0;
}

int main()
{
    csprng RNG;
    int i;
    char pr[10];
    unsigned long ran;

    time((time_t *)&ran);
    pr[0] = ran;
    pr[1] = ran >> 8;
    pr[2] = ran >> 16;
    pr[3] = ran >> 24;
    for (i = 4; i < 10; i++) pr[i] = i;
    RAND_seed(&RNG, 10, pr);

    ed25519(&RNG);
#if CHUNK==32 || CHUNK==64
    nist256(&RNG);
    ed448(&RNG);
#endif
    bn254(&RNG);
#if CHUNK==32 || CHUNK==64
    bls383(&RNG);
    bls24(&RNG);
    bls48(&RNG);
#endif
    rsa2048(&RNG);

}
