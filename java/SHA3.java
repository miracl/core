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

/*
 * Implementation of the Secure Hashing Algorithm SHA-3

 * Generates a message digest. It should be impossible to come
 * come up with two messages that hash to the same value ("collision free").
 *
 * For use with byte-oriented messages only. 
 */


package org.miracl.core;

public class SHA3 {
	private long length;
	private int rate,len;
	private long[][] S=new long[5][5];

/* Constructors */
	public SHA3(int olen)
	{
		init(olen);
	}

    public SHA3(SHA3 s)
    {
        length=s.length;
        rate=s.rate;
        len=s.len;
		for (int i=0;i<5;i++) 
			for (int j=0;j<5;j++)
                S[i][j]=s.S[i][j];
    }

	public static final int HASH224=28; 
	public static final int HASH256=32;
	public static final int HASH384=48;
	public static final int HASH512=64;

	public static final int SHAKE128=16; 
	public static final int SHAKE256=32; 

	public static final long[] RC={
		0x0000000000000001L,0x0000000000008082L,0x800000000000808AL,0x8000000080008000L,
		0x000000000000808BL,0x0000000080000001L,0x8000000080008081L,0x8000000000008009L,
		0x000000000000008AL,0x0000000000000088L,0x0000000080008009L,0x000000008000000AL,
		0x000000008000808BL,0x800000000000008BL,0x8000000000008089L,0x8000000000008003L,
		0x8000000000008002L,0x8000000000000080L,0x000000000000800AL,0x800000008000000AL,
		0x8000000080008081L,0x8000000000008080L,0x0000000080000001L,0x8000000080008008L};

	private static final int ROUNDS=24;


	private static long rotl(long x,int n)
	{
		return (((x)<<n) | ((x)>>>(64-n)));
	}

	private void transform()
	{ /* basic transformation step */
		int i,j,k;
		long[] C=new long[5];
		long[] D=new long[5];
		long[][] B=new long[5][5];

		for (k=0;k<ROUNDS;k++)
		{
			C[0]=S[0][0]^S[0][1]^S[0][2]^S[0][3]^S[0][4];
			C[1]=S[1][0]^S[1][1]^S[1][2]^S[1][3]^S[1][4];
			C[2]=S[2][0]^S[2][1]^S[2][2]^S[2][3]^S[2][4];
			C[3]=S[3][0]^S[3][1]^S[3][2]^S[3][3]^S[3][4];
			C[4]=S[4][0]^S[4][1]^S[4][2]^S[4][3]^S[4][4];

			D[0]=C[4]^rotl(C[1],1);
			D[1]=C[0]^rotl(C[2],1);
			D[2]=C[1]^rotl(C[3],1);
			D[3]=C[2]^rotl(C[4],1);
			D[4]=C[3]^rotl(C[0],1);

			for (i=0;i<5;i++)
				for (j=0;j<5;j++)
					S[i][j]^=D[i];  

			B[0][0]=S[0][0];
			B[1][3]=rotl(S[0][1],36);
			B[2][1]=rotl(S[0][2],3);
			B[3][4]=rotl(S[0][3],41);
			B[4][2]=rotl(S[0][4],18);

			B[0][2]=rotl(S[1][0],1);
			B[1][0]=rotl(S[1][1],44);
			B[2][3]=rotl(S[1][2],10);
			B[3][1]=rotl(S[1][3],45);
			B[4][4]=rotl(S[1][4],2);

			B[0][4]=rotl(S[2][0],62);
			B[1][2]=rotl(S[2][1],6);
			B[2][0]=rotl(S[2][2],43);
			B[3][3]=rotl(S[2][3],15);
			B[4][1]=rotl(S[2][4],61);

			B[0][1]=rotl(S[3][0],28);
			B[1][4]=rotl(S[3][1],55);
			B[2][2]=rotl(S[3][2],25);
			B[3][0]=rotl(S[3][3],21);
			B[4][3]=rotl(S[3][4],56);

			B[0][3]=rotl(S[4][0],27);
			B[1][1]=rotl(S[4][1],20);
			B[2][4]=rotl(S[4][2],39);
			B[3][2]=rotl(S[4][3],8);
			B[4][0]=rotl(S[4][4],14);

			for (i=0;i<5;i++)
				for (j=0;j<5;j++)
					S[i][j]=B[i][j]^(~B[(i+1)%5][j]&B[(i+2)%5][j]);

			S[0][0]^=RC[k];
		}
	} 

/* Re-Initialize. olen is output length in bytes - 
   should be 28, 32, 48 or 64 (224, 256, 384, 512 bits resp.) */


/* Initialise Hash function */
	public void init(int olen)
	{ /* initialise */
		int i,j;
		for (i=0;i<5;i++) 
			for (j=0;j<5;j++)
				S[i][j]=0;    /* 5x5x8 bytes = 200 bytes of state */
		length=0;
		len=olen;
		rate=200-2*olen; /* number of bytes consumed in one gulp. Note that some bytes in the 
	                        state ("capacity") are not touched. Gulps are smaller for larger digests. 
							Important that olen<rate */
	}

/* process a single byte */
	public void process(int byt)
	{ /* process the next message byte */
		int i,j,b,cnt;
		cnt=(int)(length%rate);
		b=cnt%8;
		cnt/=8;
		i=cnt%5; j=cnt/5;  /* process by columns! */
		S[i][j]^=((long)(byt&0xff)<<(8*b));
		length++;
		if ((length%rate)==0) transform();
	}

    public void process_array(byte[] b) {
        for (int i = 0; i < b.length; i++) process((int)b[i]);
    }

    /* process a 32-bit integer */
    public void process_num(int n) {
        process((n >> 24) & 0xff);
        process((n >> 16) & 0xff);
        process((n >> 8) & 0xff);
        process(n & 0xff);
    }

/* squeeze the sponge */
	public void squeeze(byte[] buff,int olen)
	{
		boolean done;
		int i,j,k,m=0;
		long el;
/* extract by columns */
		done=false;
		for (;;)
		{
			for (j=0;j<5;j++)
			{
				for (i=0;i<5;i++)
				{
					el=S[i][j];
					for (k=0;k<8;k++)
					{
						buff[m++]=(byte)(el&0xff);
						if (m>=olen || (m%rate)==0) {done=true; break;} 
						el>>>=8;
					}
					if (done) break;
				}
				if (done) break;
			}
			if (m>=olen) break;
			done=false;
			transform();
		}
	}

	public byte[] hash()
	{ /* generate a SHA3 hash of appropriate size */
        byte[] digest = new byte[len];
		int q=rate-(int)(length%rate);
		if (q==1) process(0x86); 
		else
		{
			process(0x06);   /* 0x06 for SHA-3 */
			while (length%rate!=rate-1) process(0x00);
			process(0x80); /* this will force a final transform */
		}
		squeeze(digest,len);
        return digest;
	}

    public byte[] continuing_hash()
    {
        SHA3 sh=new SHA3(this);
        return sh.hash();

    }

	public void shake(byte[] digest,int olen)
	{ /* SHAKE out a buffer of variable length olen */
		int q=rate-(int)(length%rate);
		if (q==1) process(0x9f); 
		else
		{
			process(0x1f);   // 0x06 for SHA-3 !!!!
			while (length%rate!=rate-1) process(0x00);
			process(0x80); /* this will force a final transform */
		}
		squeeze(digest,olen);
	}

    public void continuing_shake(byte[] digest,int olen)
    {
        SHA3 sh=new SHA3(this);
        sh.shake(digest,olen);
    }

/* test program: should produce digests */

//916f6061fe879741ca6469b43971dfdb28b1a32dc36cb3254e812be27aad1d18
//afebb2ef542e6579c50cad06d2e578f9f8dd6881d7dc824d26360feebf18a4fa73e3261122948efcfd492e74e82e2189ed0fb440d187f382270cb455f21dd185
//98be04516c04cc73593fef3ed0352ea9f6443942d6950e29a372a681c3deaf4535423709b02843948684e029010badcc0acd8303fc85fdad3eabf4f78cae165635f57afd28810fc2

/*
	public static void main(String[] args) {

		byte[] test="abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu".getBytes();
		byte[] digest=new byte[100];
		int i;

		SHA3 sh256=new SHA3(SHA3.HASH256);
		for (i=0;i<test.length;i++)
			sh256.process(test[i]);
		sh256.hash(digest);    
		for (i=0;i<32;i++) System.out.format("%02x",digest[i]);
		System.out.println("");

		SHA3 sh512=new SHA3(SHA3.HASH512);
		for (i=0;i<test.length;i++)
			sh512.process(test[i]);
		sh512.hash(digest);    
		for (i=0;i<64;i++) System.out.format("%02x",digest[i]);
		System.out.println("");

		SHA3 sk256=new SHA3(SHA3.SHAKE256);
		for (i=0;i<test.length;i++)
			sk256.process(test[i]);
		sk256.shake(digest,72);    
		for (i=0;i<72;i++) System.out.format("%02x",digest[i]);
		System.out.println("");

	}  
*/
}