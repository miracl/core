
#include <stdio.h>
#include "fp_YYY.h"

#ifdef COMBA

int main()
{
	int i,j,k,N;

	N=NLEN_XXX;

	printf("Insert this code in file fp_YYY.c\n\n");

	printf("void FP_YYY_modmul(BIG_XXX r,BIG_XXX a,BIG_XXX b)\n");

	printf("{\n");
	printf("\tdchunk t,c,s;\n");
	printf("\tdchunk d[%d],dd[%d];\n",N,N);
	printf("\tchunk v[%d],md[%d];\n",N,N);	
	printf("\tchunk MC=MConst_YYY;\n");

	printf("\tBIG_XXX_rcopy(md,Modulus_YYY);\n");

	for (i=0;i<N;i++)
		printf("\td[%d]=(dchunk)a[%d]*b[%d];\n",i,i,i);

	printf("\n\ts=d[0];  t=s; v[0]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[0]*md[0]; c=(t>>BASEBITS_XXX);\n\n");

	for (k=1;k<N;k++)
	{
		printf("\ts+=d[%d]; t=s ",k);
		for (i=k;i>=1+k/2;i--)
			printf("+(dchunk)(a[%d]-a[%d])*(b[%d]-b[%d])",i,k-i,k-i,i);
		printf("; t+=c+(dchunk)v[0]*md[%d]",k,k);
		for (i=k-1;i>k/2;i--) printf("+(dchunk)(v[%d]-v[%d])*(md[%d]-md[%d])",k-i,i,i,k-i);
		printf("; v[%d]=((chunk)t*MC)&BMASK_XXX; t+=(dchunk)v[%d]*md[0]; ",k,k);
		printf(" dd[%d]=(dchunk)v[%d]*md[%d]; s+=dd[%d]; c=(t>>BASEBITS_XXX); \n",k,k,k,k);
	}
	printf("\n");
	for (k=N;k<2*N-1;k++)
	{
		printf("\ts-=d[%d]; t=s ",k-N);
		for (i=N-1;i>=1+k/2;i--)
			printf("+(dchunk)(a[%d]-a[%d])*(b[%d]-b[%d])",i,k-i,k-i,i);
		printf("; t+=c",k);
		for (i=N-1;i>=1+k/2;i--) printf("+(dchunk)(v[%d]-v[%d])*(md[%d]-md[%d])",k-i,i,i,k-i);
		printf("; r[%d]=(chunk)t&BMASK_XXX;  s-=dd[%d]; c=(t>>BASEBITS_XXX); \n",k-N,k-N+1);
	}
	
	printf("\tr[%d]=(chunk)c&BMASK_XXX;\n",N-1);	

    printf("}\n");

}

#endif


