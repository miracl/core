/* Test and benchmark elliptic curve and RSA functions
	First build amcl.a from build_ec batch file
	gcc -O3 benchtest_ec.c amcl.a -o benchtest_ec.exe
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "rsa_2048.h"
#include "ecp_ED25519.h"
#include "pair_BN254.h"


#if CHUNK==32 || CHUNK==64
#include "ecp_NIST256.h"
#include "ecp_GOLDILOCKS.h"
#include "pair_BLS383.h"
#include "pair192_BLS24.h"
#include "pair256_BLS48.h"
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

#define BIG_BLS12 BIG_384_29
#define BIG_BLS12_rcopy BIG_384_29_rcopy
#define BIG_BLS12_randomnum BIG_384_29_randomnum 
#define BIG_BLS12_randtrunc BIG_384_29_randtrunc 

#define BIG_BLS24 BIG_480_29
#define BIG_BLS24_rcopy BIG_480_29_rcopy
#define BIG_BLS24_randomnum BIG_480_29_randomnum 
#define BIG_BLS24_randtrunc BIG_480_29_randtrunc

#define BIG_BLS48 BIG_560_29
#define BIG_BLS48_rcopy BIG_560_29_rcopy
#define BIG_BLS48_randomnum BIG_560_29_randomnum 
#define BIG_BLS48_randtrunc BIG_560_29_randtrunc 

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

#define BIG_BLS24 BIG_480_56
#define BIG_BLS24_rcopy BIG_480_56_rcopy
#define BIG_BLS24_randomnum BIG_480_56_randomnum 
#define BIG_BLS24_randtrunc BIG_480_56_randtrunc

#define BIG_BLS48 BIG_560_58
#define BIG_BLS48_rcopy BIG_560_58_rcopy
#define BIG_BLS48_randomnum BIG_560_58_randomnum 
#define BIG_BLS48_randtrunc BIG_560_58_randtrunc 

#endif

int ED_25519(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;
	ECP_ED25519 EP,EG;
	BIG_ED s,r,x,y;
	printf("\nTesting/Timing ED25519 ECC\n");

#if CURVETYPE_ED25519==WEIERSTRASS
	printf("Weierstrass parameterization\n");
#endif
#if CURVETYPE_ED25519==EDWARDS
	printf("Edwards parameterization\n");
#endif
#if CURVETYPE_ED25519==MONTGOMERY
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
	
	ECP_ED25519_generator(&EG);

	BIG_ED_rcopy(r,CURVE_Order_ED25519);
	BIG_ED_randtrunc(s,r,2*CURVE_SECURITY_ED25519,RNG);
	ECP_ED25519_copy(&EP,&EG);
    ECP_ED25519_mul(&EP,r);

	if (!ECP_ED25519_isinf(&EP))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP_ED25519_copy(&EP,&EG);
		ECP_ED25519_mul(&EP,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("EC  mul - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	return 0;
}

#if CHUNK==32 || CHUNK==64

int NIST_256(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;
	ECP_NIST256 EP,EG;
	BIG_NT s,r,x,y;
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

	BIG_NT_rcopy(r,CURVE_Order_NIST256);
	BIG_NT_randtrunc(s,r,2*CURVE_SECURITY_NIST256,RNG);
	ECP_NIST256_copy(&EP,&EG);
    ECP_NIST256_mul(&EP,r);

	if (!ECP_NIST256_isinf(&EP))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP_NIST256_copy(&EP,&EG);
		ECP_NIST256_mul(&EP,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("EC  mul - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	return 0;
}

int GOLD_LOCKS(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;
	ECP_GOLDILOCKS EP,EG;
	BIG_GL s,r,x,y;
	printf("\nTesting/Timing GOLDILOCKS ECC\n");

#if CURVETYPE_GOLDILOCKS==WEIERSTRASS
	printf("Weierstrass parameterization\n");
#endif
#if CURVETYPE_GOLDILOCKS==EDWARDS
	printf("Edwards parameterization\n");
#endif
#if CURVETYPE_GOLDILOCKS==MONTGOMERY
	printf("Montgomery parameterization\n");
#endif

#if MODTYPE_GOLDILOCKS == PSEUDO_MERSENNE
	printf("Pseudo-Mersenne Modulus\n");
#endif

#if MODTYPE_GOLDILOCKS == GENERALISED_MERSENNE
	printf("Generalised-Mersenne Modulus\n");
#endif

#if MODTYPE_GOLDILOCKS == MONTGOMERY_FRIENDLY
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
	
	ECP_GOLDILOCKS_generator(&EG);

	BIG_GL_rcopy(r,CURVE_Order_GOLDILOCKS);
	BIG_GL_randtrunc(s,r,2*CURVE_SECURITY_GOLDILOCKS,RNG);
	ECP_GOLDILOCKS_copy(&EP,&EG);
    ECP_GOLDILOCKS_mul(&EP,r);

	if (!ECP_GOLDILOCKS_isinf(&EP))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP_GOLDILOCKS_copy(&EP,&EG);
		ECP_GOLDILOCKS_mul(&EP,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("EC  mul - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	return 0;
}
#endif

int BN_254(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;

	ECP_BN254 P,G;
	ECP2_BN254 Q,W;
	FP12_BN254 g,w;
	FP4_BN254 cm;

	BIG_BN s,r,x,y;
	printf("\nTesting/Timing BN254 Pairings\n");

	ECP_BN254_generator(&G);

	
	BIG_BN_rcopy(r,CURVE_Order_BN254);
	BIG_BN_randtrunc(s,r,2*CURVE_SECURITY_BN254,RNG);
	ECP_BN254_copy(&P,&G);
    PAIR_BN254_G1mul(&P,r);

	if (!ECP_BN254_isinf(&P))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}
	
	iterations=0;
    start=clock();
    do {
		ECP_BN254_copy(&P,&G);
		PAIR_BN254_G1mul(&P,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G1 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP2_BN254_generator(&W);

	ECP2_BN254_copy(&Q,&W);
    ECP2_BN254_mul(&Q,r);

	if (!ECP2_BN254_isinf(&Q))
	{
		printf("FAILURE - rQ!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP2_BN254_copy(&Q,&W);
		PAIR_BN254_G2mul(&Q,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G2 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	PAIR_BN254_ate(&w,&Q,&P);
	PAIR_BN254_fexp(&w);

	FP12_BN254_copy(&g,&w);

	PAIR_BN254_GTpow(&g,r);

	if (!FP12_BN254_isunity(&g))
	{
		printf("FAILURE - g^r!=1\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		FP12_BN254_copy(&g,&w);
		PAIR_BN254_GTpow(&g,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	FP12_BN254_copy(&g,&w);

	iterations=0;
    start=clock();
    do {
		FP12_BN254_compow(&cm,&g,s,r);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow (compressed) - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		PAIR_BN254_ate(&w,&Q,&P);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing ATE         - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		FP12_BN254_copy(&g,&w);
		PAIR_BN254_fexp(&g);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing FEXP        - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP_BN254_copy(&P,&G);	
	ECP2_BN254_copy(&Q,&W);

	PAIR_BN254_G1mul(&P,s);
	PAIR_BN254_ate(&g,&Q,&P);
	PAIR_BN254_fexp(&g);

	ECP_BN254_copy(&P,&G);

	PAIR_BN254_G2mul(&Q,s);
	PAIR_BN254_ate(&w,&Q,&P);
	PAIR_BN254_fexp(&w);

	if (!FP12_BN254_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
		return 0;
	}

	ECP2_BN254_copy(&Q,&W);
	PAIR_BN254_ate(&g,&Q,&P);
	PAIR_BN254_fexp(&g);

	PAIR_BN254_GTpow(&g,s);

	if (!FP12_BN254_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
		return 0;
	}
	return 0;
}

#if CHUNK==32 || CHUNK==64

int BLS_383(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;

	ECP_BLS383 P,G;
	ECP2_BLS383 Q,W;
	FP12_BLS383 g,w;
	FP4_BLS383 cm;

	BIG_BLS12 s,r,x,y;
	printf("\nTesting/Timing BLS383 Pairings\n");

	ECP_BLS383_generator(&G);

	
	BIG_BLS12_rcopy(r,CURVE_Order_BLS383);
	BIG_BLS12_randtrunc(s,r,2*CURVE_SECURITY_BLS383,RNG);
	ECP_BLS383_copy(&P,&G);
    PAIR_BLS383_G1mul(&P,r);

	if (!ECP_BLS383_isinf(&P))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}
	
	iterations=0;
    start=clock();
    do {
		ECP_BLS383_copy(&P,&G);
		PAIR_BLS383_G1mul(&P,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G1 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP2_BLS383_generator(&W);

	ECP2_BLS383_copy(&Q,&W);
    ECP2_BLS383_mul(&Q,r);

	if (!ECP2_BLS383_isinf(&Q))
	{
		printf("FAILURE - rQ!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP2_BLS383_copy(&Q,&W);
		PAIR_BLS383_G2mul(&Q,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G2 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	PAIR_BLS383_ate(&w,&Q,&P);
	PAIR_BLS383_fexp(&w);

	FP12_BLS383_copy(&g,&w);

	PAIR_BLS383_GTpow(&g,r);

	if (!FP12_BLS383_isunity(&g))
	{
		printf("FAILURE - g^r!=1\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		FP12_BLS383_copy(&g,&w);
		PAIR_BLS383_GTpow(&g,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	FP12_BLS383_copy(&g,&w);

	iterations=0;
    start=clock();
    do {
		FP12_BLS383_compow(&cm,&g,s,r);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow (compressed) - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		PAIR_BLS383_ate(&w,&Q,&P);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing ATE         - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		FP12_BLS383_copy(&g,&w);
		PAIR_BLS383_fexp(&g);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing FEXP        - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP_BLS383_copy(&P,&G);	
	ECP2_BLS383_copy(&Q,&W);

	PAIR_BLS383_G1mul(&P,s);
	PAIR_BLS383_ate(&g,&Q,&P);
	PAIR_BLS383_fexp(&g);

	ECP_BLS383_copy(&P,&G);

	PAIR_BLS383_G2mul(&Q,s);
	PAIR_BLS383_ate(&w,&Q,&P);
	PAIR_BLS383_fexp(&w);

	if (!FP12_BLS383_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
		return 0;
	}

	ECP2_BLS383_copy(&Q,&W);
	PAIR_BLS383_ate(&g,&Q,&P);
	PAIR_BLS383_fexp(&g);

	PAIR_BLS383_GTpow(&g,s);

	if (!FP12_BLS383_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
		return 0;
	}
	return 0;
}

int BLS_24(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;

	ECP_BLS24 P,G;
	ECP4_BLS24 Q,W;
	FP24_BLS24 g,w;
	FP8_BLS24 cm;

	BIG_BLS24 s,r,x,y;

	printf("\nTesting/Timing BLS24 Pairings\n");

	ECP_BLS24_generator(&G);

	
	BIG_BLS24_rcopy(r,CURVE_Order_BLS24);
	BIG_BLS24_randtrunc(s,r,2*CURVE_SECURITY_BLS24,RNG);
	ECP_BLS24_copy(&P,&G);
    PAIR_BLS24_G1mul(&P,r);

	if (!ECP_BLS24_isinf(&P))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}
	
	iterations=0;
    start=clock();
    do {
		ECP_BLS24_copy(&P,&G);
		PAIR_BLS24_G1mul(&P,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G1 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP4_BLS24_generator(&W);

	ECP4_BLS24_copy(&Q,&W);
    ECP4_BLS24_mul(&Q,r);

	if (!ECP4_BLS24_isinf(&Q))
	{
		printf("FAILURE - rQ!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP4_BLS24_copy(&Q,&W);
		PAIR_BLS24_G2mul(&Q,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G2 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	PAIR_BLS24_ate(&w,&Q,&P);
	PAIR_BLS24_fexp(&w);

	FP24_BLS24_copy(&g,&w);

	PAIR_BLS24_GTpow(&g,r);

	if (!FP24_BLS24_isunity(&g))
	{
		printf("FAILURE - g^r!=1\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		FP24_BLS24_copy(&g,&w);
		PAIR_BLS24_GTpow(&g,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	FP24_BLS24_copy(&g,&w);

	iterations=0;
    start=clock();
    do {
		FP24_BLS24_compow(&cm,&g,s,r);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow (compressed) - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		PAIR_BLS24_ate(&w,&Q,&P);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing ATE         - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		FP24_BLS24_copy(&g,&w);
		PAIR_BLS24_fexp(&g);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing FEXP        - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP_BLS24_copy(&P,&G);	
	ECP4_BLS24_copy(&Q,&W);

	PAIR_BLS24_G1mul(&P,s);
	PAIR_BLS24_ate(&g,&Q,&P);
	PAIR_BLS24_fexp(&g);

	ECP_BLS24_copy(&P,&G);

	PAIR_BLS24_G2mul(&Q,s);
	PAIR_BLS24_ate(&w,&Q,&P);
	PAIR_BLS24_fexp(&w);

	if (!FP24_BLS24_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
		return 0;
	}

	ECP4_BLS24_copy(&Q,&W);
	PAIR_BLS24_ate(&g,&Q,&P);
	PAIR_BLS24_fexp(&g);

	PAIR_BLS24_GTpow(&g,s);

	if (!FP24_BLS24_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
		return 0;
	}
	return 0;
}


int BLS_48(csprng *RNG)
{
    int i,iterations;
    clock_t start;
    double elapsed;

	ECP_BLS48 P,G;
	ECP8_BLS48 Q,W;
	FP48_BLS48 g,w;
	FP16_BLS48 cm;

	BIG_BLS48 s,r,x,y;
	printf("\nTesting/Timing BLS48 Pairings\n");

	ECP_BLS48_generator(&G);

	
	BIG_BLS48_rcopy(r,CURVE_Order_BLS48);
	BIG_BLS48_randtrunc(s,r,2*CURVE_SECURITY_BLS48,RNG);
	ECP_BLS48_copy(&P,&G);
    PAIR_BLS48_G1mul(&P,r);

	if (!ECP_BLS48_isinf(&P))
	{
		printf("FAILURE - rG!=O\n");
		return 0;
	}
	
	iterations=0;
    start=clock();
    do {
		ECP_BLS48_copy(&P,&G);
		PAIR_BLS48_G1mul(&P,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G1 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP8_BLS48_generator(&W);

	ECP8_BLS48_copy(&Q,&W);
    ECP8_BLS48_mul(&Q,r);

	if (!ECP8_BLS48_isinf(&Q))
	{
		printf("FAILURE - rQ!=O\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		ECP8_BLS48_copy(&Q,&W);
		PAIR_BLS48_G2mul(&Q,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("G2 mul              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	PAIR_BLS48_ate(&w,&Q,&P);
	PAIR_BLS48_fexp(&w);

	FP48_BLS48_copy(&g,&w);

	PAIR_BLS48_GTpow(&g,r);

	if (!FP48_BLS48_isunity(&g))
	{
		printf("FAILURE - g^r!=1\n");
		return 0;
	}

	iterations=0;
    start=clock();
    do {
		FP48_BLS48_copy(&g,&w);
		PAIR_BLS48_GTpow(&g,s);

		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow              - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	FP48_BLS48_copy(&g,&w);

	iterations=0;
    start=clock();
    do {
		FP48_BLS48_compow(&cm,&g,s,r);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("GT pow (compressed) - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		PAIR_BLS48_ate(&w,&Q,&P);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing ATE         - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		FP48_BLS48_copy(&g,&w);
		PAIR_BLS48_fexp(&g);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("PAIRing FEXP        - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	ECP_BLS48_copy(&P,&G);	
	ECP8_BLS48_copy(&Q,&W);

	PAIR_BLS48_G1mul(&P,s);
	PAIR_BLS48_ate(&g,&Q,&P);
	PAIR_BLS48_fexp(&g);

	ECP_BLS48_copy(&P,&G);

	PAIR_BLS48_G2mul(&Q,s);
	PAIR_BLS48_ate(&w,&Q,&P);
	PAIR_BLS48_fexp(&w);

	if (!FP48_BLS48_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,sP) \n");
		return 0;
	}

	ECP8_BLS48_copy(&Q,&W);
	PAIR_BLS48_ate(&g,&Q,&P);
	PAIR_BLS48_fexp(&g);

	PAIR_BLS48_GTpow(&g,s);

	if (!FP48_BLS48_equals(&g,&w))
	{
		printf("FAILURE - e(sQ,p)!=e(Q,P)^s \n");
		return 0;
	}
	return 0;
}
#endif

int RSA_2048(csprng *RNG)
{
    rsa_public_key_2048 pub;
    rsa_private_key_2048 priv;

    int i,iterations;
    clock_t start;
    double elapsed;

    char m[RFS_2048],d[RFS_2048],c[RFS_2048];
    octet M= {0,sizeof(m),m};
    octet D= {0,sizeof(d),d};
    octet C= {0,sizeof(c),c};

	printf("\nTesting/Timing 2048-bit RSA\n");

	printf("Generating 2048-bit RSA public/private key pair\n");

	iterations=0;
    start=clock();
    do {
		RSA_2048_KEY_PAIR(RNG,65537,&priv,&pub,NULL,NULL);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("RSA gen - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	M.len=RFS_2048;
	for (i=0;i<RFS_2048;i++) M.val[i]=i%128;

	iterations=0;
    start=clock();
    do {
		RSA_2048_ENCRYPT(&pub,&M,&C);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("RSA enc - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	iterations=0;
    start=clock();
    do {
		RSA_2048_DECRYPT(&priv,&C,&D);
		iterations++;
		elapsed=(clock()-start)/(double)CLOCKS_PER_SEC;
    } while (elapsed<MIN_TIME || iterations<MIN_ITERS);
    elapsed=1000.0*elapsed/iterations;
    printf("RSA dec - %8d iterations  ",iterations);
    printf(" %8.2lf ms per iteration\n",elapsed);

	for (i=0;i<RFS_2048;i++)
	{
		if (M.val[i]!=D.val[i])
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
	pr[0]=ran;
	pr[1]=ran>>8;
	pr[2]=ran>>16;
	pr[3]=ran>>24;
	for (i=4;i<10;i++) pr[i]=i;
    RAND_seed(&RNG,10,pr);

	ED_25519(&RNG);
#if CHUNK==32 || CHUNK==64	
	NIST_256(&RNG);
	GOLD_LOCKS(&RNG);
#endif	
	BN_254(&RNG);
#if CHUNK==32 || CHUNK==64	
	BLS_383(&RNG);
	BLS_24(&RNG);
	BLS_48(&RNG);
#endif	
	RSA_2048(&RNG); 
	
}
