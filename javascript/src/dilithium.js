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

/* Kyber API high-level functions  */

var DILITHIUM = function(ctx) {
    "use strict";

    var DILITHIUM = {

        round: function(a, b) {
            return Math.floor((a + (b>>1)) / b);
        },

    /* constant time absolute vaue */
        nabs: function(x) {
            var mask = (x >> 31);
            return (x + mask) ^ mask;
        },

        mul32: function(a,b) {
            var abot=a&0xffff;
            var atop=a>>>16;
            var bbot=b&0xffff;
            var btop=b>>>16;

            var pp1=abot*bbot;
            var pp2=abot*btop+atop*bbot;
            var pp3=atop*btop;

            var bot=(pp1>>>0)+(((pp2&0xffff)<<16)>>>0);
            var top=(pp3>>>0)+((pp2>>>16)>>>0);

            var t=bot;
            bot = bot >>> 0;

            //var diff=t-bot; diff/=2; diff>>>=31;
            //top+=diff;

            if (t!=bot)
                top=(top+1);

            return new ctx.UInt64(top,bot);
        },

        mul32b: function(a,b) {
            var abot=a&0xffff;
            var atop=a>>>16;
            var bbot=b&0xffff;
            var btop=b>>>16;

            var pp1=abot*bbot;
            var pp2=abot*btop+atop*bbot;

            var bot=pp1+((pp2&0xffff)<<16);
            bot >>>= 0;

            return bot;
        },

    /* Montgomery stuff */
        redc: function(T) {
            var m=DILITHIUM.mul32b(T.bot,DILITHIUM.ND);  // only return bottom half
            var t=DILITHIUM.mul32(m,DILITHIUM.PRIME);
            t.add(T);
            return t.top;
        },

        nres: function(x) {
            return DILITHIUM.redc(DILITHIUM.mul32(x,DILITHIUM.R2MODP)); 
        },

        modmul: function(a, b) {
            var m=DILITHIUM.mul32(a,b);
            return DILITHIUM.redc(m);
        },

// make all elements +ve
        poly_pos: function(p) {
            for (var i=0;i<DILITHIUM.DEGREE;i++)
                p[i]+=(p[i]>>31)&DILITHIUM.PRIME; 
        },

// NTT code

// Important!
// nres(x); ntt(x)
// nres(y); ntt(y)
// z=x*y
// intt(z);
// redc(z);

// is equivalent to (note that nres() and redc() cancel out)

// ntt(x);
// nres(y); ntt(y);
// z=x*y
// intt(z)

// is equivalent to

// ntt(x)
// ntt(y)
// z=x*y
// intt(z)
// nres(z)

// In all cases z ends up in normal (non-Montgomery) form!
// So the conversion to Montgomery form can be "pushed" through the calculation.

// Here intt(z) <- intt(z);nres(z); 
// Combining is more efficient
// note that ntt() and intt() are not mutually inverse

/* NTT code */
/* Cooley-Tukey NTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */

        ntt: function(x) {
            var len=DILITHIUM.DEGREE/2;
            var q=DILITHIUM.PRIME

    /* Make positive */
            DILITHIUM.poly_pos(x);
//console.log("ppos= "+DILITHIUM.wordstostring(x));
            var m = 1;
            while (m < DILITHIUM.DEGREE)
            {
                var start = 0;
                for ( var i = 0; i < m; i++)
                {
                    var S = DILITHIUM.roots[m + i];
                    for (var j = start; j < start + len; j++)
                    {
                        var V = DILITHIUM.modmul(x[j + len], S);
//console.log("V= "+V+" "+x[j+len]+" "+S);
                        x[j + len] = x[j] + 2 * q - V;
                        x[j] = x[j] + V;
                    }
                    start += 2 * len;
                }
                len /= 2;
                m *= 2;
            }
        },

/* Gentleman-Sande INTT */
/* Excess of 2 allowed on input - coefficients must be < 2*PRIME */
/* Output fully reduced */

        intt(x) {
            var t=1;
            var q=DILITHIUM.PRIME
            var m=DILITHIUM.DEGREE/2;
            var n=DILITHIUM.LGN;
            while (m >= 1)
            {
    		    var lim=DILITHIUM.NTTL>>n;
    		    n--;
                var k = 0;
                for (var i = 0; i < m; i++)
                {
                    var S = DILITHIUM.iroots[m + i];
                    for (var j = k; j < k + t; j++)
                    {
                        var U,V;
                        if (DILITHIUM.NTTL>1) {
				            if (m<DILITHIUM.NTTL && j<k+lim)   // This should be unwound for timings
				            { // need to knock back excesses. Never used if NTTL=1.
				    	        U=DILITHIUM.modmul(x[j],DILITHIUM.ONE);  
				    	        V=DILITHIUM.modmul(x[j+t],DILITHIUM.ONE); 
				            } else {
                                U = x[j];
                                V = x[j + t];
                            }
                        } else {
                            U = x[j];
                            V = x[j + t];                
                        }
                        x[j] = U + V;
                        var W = U + (DILITHIUM.DEGREE/DILITHIUM.NTTL) * q - V; 
                        x[j + t] = DILITHIUM.modmul(W, S); 
                    }
                    k += 2 * t;
                }
                t *= 2;
                m /= 2;
            }

            for (j = 0; j < DILITHIUM.DEGREE; j++)
            { // fully reduce, nres combined with 1/DEGREE
                x[j]=  DILITHIUM.modmul(x[j],DILITHIUM.COMBO);
                x[j] -= q;
                x[j] += (x[j] >> 31)&q;
            } 
        },

        nres_it: function(p) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p[i] = DILITHIUM.nres(p[i]);
        },

        redc_it: function(p) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++) {
                var m =new ctx.UInt64(0,p[i]);
                p[i] = DILITHIUM.redc(m);
            }
        },
// copy polynomial
        poly_copy: function(p1, p2) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = p2[i];
        },

        poly_scopy: function(p1, p2) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = p2[i] << 24 >>24;
        },

        poly_zero: function(p1) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = 0;
        },

        poly_negate: function(p1,p2) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = DILITHIUM.PRIME-p2[i];
        },

        poly_mul: function(p1, p2, p3) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = DILITHIUM.modmul(p2[i], p3[i]);
        },

        poly_add: function(p1, p2, p3) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = (p2[i] + p3[i]);
        },

        poly_sub: function(p1, p2, p3) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
                p1[i] = (p2[i] + DILITHIUM.PRIME - p3[i]);
        },

/* reduces inputs that are already < 2q */
        poly_soft_reduce: function(poly) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
            {
                var e = poly[i] - DILITHIUM.PRIME;
                poly[i] = e + ((e >> 31)&DILITHIUM.PRIME);
            }
        },

/* fully reduces modulo q */
        poly_hard_reduce: function(poly) {
            for (var i = 0; i < DILITHIUM.DEGREE; i++)
            {
                var e = DILITHIUM.modmul(poly[i], DILITHIUM.ONE); // reduces to < 2q
                e = e - DILITHIUM.PRIME;
                poly[i] = e + ((e >> 31)&DILITHIUM.PRIME); // finishes it off
            }
        },

// Generate A[i][j] from rho
        ExpandAij: function(rho,Aij,i,j)
        {
		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE128);
            var buff = new Uint8Array(840);   // should be plenty
            for (var m=0;m<32;m++)
                sh.process(rho[m]);
            sh.process(j&0xff);
            sh.process(i&0xff);
            sh.shake(buff,840);
            var m=0;
            var n=0;
//console.log("buff[0]= "+buff[0]+" "+buff[1]+" "+buff[2]+" "+buff[3]+" "+buff[4]+" "+buff[5]);
            while (m<DILITHIUM.DEGREE)
            {
                var b0=(buff[n++])&0xff; 
                var b1=(buff[n++])&0xff; 
                var b2=(buff[n++])&0xff; 
                var cf=((b2&0x7F)<<16)+(b1<<8)+b0;
                if (cf>=DILITHIUM.PRIME) continue;
                Aij[m++]=cf;
            }
        },

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
        nextbyte: function(ab,max,t,position) {
	        var ptr=position[0]; // index in array
	        var bts=position[1]; // bit index in word
            var left=ab-bts; // number of bits left in this word
            var i=0;
            var k=ptr%256;
            var w=t[k];
            if (max!=0)
                w=max-w;
            var r=w>>>bts;
            while (left<8)
            {
                i++;
                w=t[k+i];
                if (max!=0)
                    w=max-w;
                r|=w<<left;
                left+=ab;
            }

            bts+=8;
            while (bts>=ab)
            {
                bts-=ab;
                ptr++;
            }
	        position[0]=ptr;
	        position[1]=bts;
            return (r&0xff);        
        },

// extract ab bits into word from dense byte stream
        nextword: function(ab,max,t,position) {
		    var ptr=position[0]; // index in array
		    var bts=position[1]; // bit index in word
            var r=(t[ptr]&0xff)>>bts;
            var mask=(1<<ab)-1;
            var i=0;
            var gotbits=8-bts; // bits left in current byte
            while (gotbits<ab)
            {
                i++;
                var w=(t[ptr+i])&0xff;
                r|=w<<gotbits;
                gotbits+=8;
            }
            bts+=ab;
            while (bts>=8)
            {
                bts-=8;
                ptr++;
            }
            var w=r&mask;
            if (max!=0)
                w=max-w;
		    position[0]=ptr;
		    position[1]=bts;
            return w;  
        },

// pack public key
        pack_pk: function(params,pk,rho,t1) {
		    var pos=[];
		    pos[0]=pos[1]=0;
            var n=0;
            var ck=params[3];
            for (var i=0;i<32;i++)
                pk[i]=rho[i];
            n=32; 
            for (var j=0;j<ck;j++)
            {
                for (i=0;i<(DILITHIUM.DEGREE*DILITHIUM.TD)/8;i++ )
                    pk[n++]=DILITHIUM.nextbyte(DILITHIUM.TD,0,t1[j],pos);
            }
            return n;
        },

// unpack public key
        unpack_pk: function(params,rho,t1,pk) {
		    var pos=[];
		    pos[0]=32; pos[1]=0;
            var ck=params[3];
            for (var i=0;i<32;i++)
                rho[i]=pk[i];
            for (var j=0;j<ck;j++)
            {
                for (var i=0;i<DILITHIUM.DEGREE;i++ )
                    t1[j][i]=DILITHIUM.nextword(DILITHIUM.TD,0,pk,pos);
            }
        },

// secret key of size 32*3+DEGREE*(K*D+L*LG2ETA1+K*LG2ETA1)/8
        pack_sk: function(params,sk,rho,bK,tr,s1,s2,t0) {
            var n=32;
            var ck=params[3];
            var el=params[4];
            var eta=params[5];
            var lg2eta1=params[6];

            for (var i=0;i<32;i++)
                sk[i]=rho[i];
            for (var i=0;i<32;i++)
                sk[n++]=bK[i];
            for (var i=0;i<32;i++)
                sk[n++]=tr[i];
		    var pos=[];
		    pos[0]=pos[1]=0;
            for (var j=0;j<el;j++)
            {
                for (var i=0;i<(DILITHIUM.DEGREE*lg2eta1)/8;i++)
                    sk[n++]=DILITHIUM.nextbyte(lg2eta1,eta,s1[j],pos);
            }
            pos[0]=pos[1]=0;
            for (var j=0;j<ck;j++)
            {
                for (i=0;i<(DILITHIUM.DEGREE*lg2eta1)/8;i++)
                    sk[n++]=DILITHIUM.nextbyte(lg2eta1,eta,s2[j],pos);
            }
            pos[0]=pos[1]=0;
            for (var j=0;j<ck;j++)
            {
                for (i=0;i<(DILITHIUM.DEGREE*DILITHIUM.D)/8;i++)
                    sk[n++]=DILITHIUM.nextbyte(DILITHIUM.D,(1<<(DILITHIUM.D-1)),t0[j],pos);
            }
            return n;
        },

        unpack_sk: function(params,rho,bK,tr,s1,s2,t0,sk) {
            var n=32;
            var ck=params[3];
            var el=params[4];
            var eta=params[5];
            var lg2eta1=params[6];

            for (var i=0;i<32;i++)
                rho[i]=sk[i];
            for (var i=0;i<32;i++)
                bK[i]=sk[n++];
            for (var i=0;i<32;i++)
                tr[i]=sk[n++];
		    var pos=[];
		    pos[0]=n; pos[1]=0;
            for (var j=0;j<el;j++)
            {
                for (var i=0;i<DILITHIUM.DEGREE;i++ )
                    s1[j][i]=DILITHIUM.nextword(lg2eta1,eta,sk,pos);
            }

//System.out.printf("lg2eta1 = %d eta= %d\n",lg2eta1,eta);
//System.out.printf("s1[0][0]= %d sk[0] = %d\n",s1[0][0],sk[n]);

            for (var j=0;j<ck;j++)
            {
                for (var i=0;i<DILITHIUM.DEGREE;i++ )
                    s2[j][i]=DILITHIUM.nextword(lg2eta1,eta,sk,pos);
            }
            for (var j=0;j<ck;j++)
            {
                for (var i=0;i<DILITHIUM.DEGREE;i++ )
                    t0[j][i]=DILITHIUM.nextword(DILITHIUM.D,(1<<(DILITHIUM.D-1)),sk,pos);
            }
        },

// pack signature - changes z 
        pack_sig: function(params,sig,z,ct,h) {
            var lg=params[1];
            var gamma1=1<<lg;
            var ck=params[3];
            var el=params[4];
            var omega=params[7];
            for (var i=0;i<32;i++)
                sig[i]=ct[i];   
            var n=32;
		    var pos=[];
		    pos[0]=pos[1]=0;
//pre-process z
            for (var i=0;i<el;i++)
            {
                for (var m=0;m<DILITHIUM.DEGREE;m++)
                {
                    var t=z[i][m];
                    if (t>DILITHIUM.PRIME/2) t-=DILITHIUM.PRIME;
                    t=gamma1-t;
                    z[i][m]=t;
                }
            }
            for (var j=0;j<el;j++)
            {
                for (var i=0;i<(DILITHIUM.DEGREE*(lg+1))/8;i++) 
                    sig[n++]=DILITHIUM.nextbyte(lg+1,0,z[j],pos);
            }
            for (var i=0;i<omega+ck;i++)
                sig[n++]=h[i];
            return n;
        },

        unpack_sig: function(params,z,ct,h,sig) {
            var lg=params[1];
            var gamma1=1<<lg;
            var ck=params[3];
            var el=params[4];
            var omega=params[7];

            var m=32+(el*DILITHIUM.DEGREE*(lg+1))/8;
            for (var i=0;i<32;i++)
                ct[i]=sig[i];

		    var pos=[];
		    pos[0]=32; pos[1]=0;
            for (var j=0;j<el;j++)
            {
                for (var i=0;i<DILITHIUM.DEGREE;i++) {
                    var t=DILITHIUM.nextword(lg+1,0,sig,pos);
                    t=gamma1-t;
                    if (t<0) t+=DILITHIUM.PRIME;
                    z[j][i]=t;
                }
            }
            for (var i=0;i<omega+ck;i++)
                h[i]=sig[m++];    
        },

// rejection sampling in range -ETA to +ETA
	    sample_Sn: function(params,rhod,s,n) {
		    var eta=params[5];
		    var lg2eta1=params[6];
		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
            var buff = new Uint8Array(272);   // should be plenty
		    for (var m=0;m<64;m++)
			    sh.process(rhod[m]);
		    sh.process(n&0xff);
		    sh.process((n>>8)&0xff);
		    sh.shake(buff,272);
		    var pos=[];
		    pos[0]=0; pos[1]=0;
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
		    {
			    do
			    {
				    s[m]=DILITHIUM.nextword(lg2eta1,0,buff,pos);
			    }
			    while (s[m]>2*eta);
			    s[m]=(eta-s[m])&0xff;
		    }
	    },

// uniform random sampling
	    sample_Y: function(params,k,rhod,y) {
		    var lg=params[1];
		    var gamma1=1<<lg;
		    var el=params[4];
		    var buff = new Uint8Array(DILITHIUM.YBYTES);

		    for (var i=0;i<el;i++)
		    {
			    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
			    for (var j=0;j<64;j++)
				    sh.process(rhod[j]);
			    var ki=k+i;
			    sh.process(ki&0xff);
			    sh.process(ki>>8);
			    sh.shake(buff,DILITHIUM.YBYTES);

			    var pos=[];
			    pos[0]=0; pos[1]=0;
			    for (var m=0;m<DILITHIUM.DEGREE;m++)
			    {  // take in LG+1 bits at a time
				    var w=DILITHIUM.nextword(lg+1,0,buff,pos);  // 20 bits
				    w=gamma1-w;
				    var t=w>>31;
				    y[i][m]=w+(DILITHIUM.PRIME&t);
			    }
		    }
	    },

// CRH(rho,t1)
	    CRH1: function(params,H,rho,t1) {
		    var pos=[];
		    pos[0]=0; pos[1]=0;
		    var ck=params[3];
		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
		    for (var i=0;i<32;i++)
			    sh.process(rho[i]);
		    for (var j=0;j<ck;j++)
		    {
			    for (var i=0;i<(DILITHIUM.DEGREE*DILITHIUM.TD)/8;i++)
                {
                    var nxt=DILITHIUM.nextbyte(DILITHIUM.TD,0,t1[j],pos);
			        sh.process(nxt);
                }
		    }
		    sh.shake(H,32);
	    },

// CRH(tr,M)
	    CRH2: function(H,tr,mess,mlen) {
		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
		    for (var i=0;i<32;i++)
			    sh.process(tr[i]);
		    for (var i=0;i<mlen;i++)
			    sh.process(mess[i]);
		    sh.shake(H,64);
	    },

// CRH(K,mu)
	    CRH3: function(H,bK,mu) {
		    var  sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
		    for (var i=0;i<32;i++)
			    sh.process(bK[i]);
		    for (var i=0;i<64;i++)
			    sh.process(mu[i]);
		    sh.shake(H,64);
	    },

// H(mu,w1)
	    H4: function(params,CT,mu,w1) {
		    var pos=[];
		    pos[0]=0; pos[1]=0;
		    var ck=params[3];
		    var dv=params[2];
		    var w1b=4;
		    if (dv==88) w1b=6;
		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
		    for (var i=0;i<64;i++)
			    sh.process(mu[i]);
		    for (var j=0;j<ck;j++)
		    {
			    for (var i=0;i<(DILITHIUM.DEGREE*w1b)/8;i++)
				    sh.process(DILITHIUM.nextbyte(w1b,0,w1[j],pos));
		    }
		    sh.shake(CT,32);
	    },

	    SampleInBall: function(params,ct,c) {
		    var tau=params[0];
		    var signs=new Uint8Array(8);
		    var buff=new Uint8Array(136);
 		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
		    for (var i=0;i<32;i++)
			    sh.process(ct[i]);
		    sh.shake(buff,136);
		    for (var i=0;i<8;i++)
			    signs[i]=buff[i];
		    var k=8; 
		    var b=0;
		    DILITHIUM.poly_zero(c);
		    var sn=(signs[0])&0xff; var n=1;
		    for (var i=DILITHIUM.DEGREE-tau;i<DILITHIUM.DEGREE;i++)
		    {
                var j;
			    do
			    {
				    j=(buff[k++])&0xff;
			    } while (j>i);
			    c[i]=c[j];
			    c[j]=1-2*(sn&1);
			    sn>>=1; b++;
			    if (b==8) {
				    sn=(signs[n++])&0xff; b=0;
			    }
		    }	
	    },

	    Power2Round: function(t,t0,t1) {
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
		    {   
			    var w=t[m];
			    var d=(1<<DILITHIUM.D);
			    var r=(w+d/2-1)>>DILITHIUM.D;
			    w-=(r<<DILITHIUM.D);
			    t1[m]=r;
			    t0[m]=w;
		    }
	    },

// ALPHA = (Q-1)/16 - borrowed from dilithium ref implementation
	    decompose_lo:function(params,a) {
		    var gamma2,dv=params[2];
		    var a0,a1;
		    a1  = (a + 127) >> 7;

		    if (dv==32)
		    {
			    a1  = (a1*1025 + (1 << 21)) >> 22;
			    a1 &= 15;
			    gamma2=(DILITHIUM.PRIME-1)/32;
		    } else { // 88
			    a1  = (a1*11275 + (1 << 23)) >> 24;
			    a1 ^= ((43 - a1) >> 31) & a1;
			    gamma2=(DILITHIUM.PRIME-1)/88;
		    }

		    a0  = a - a1*2*gamma2;  // (Q-1)/2R=alpha
		    a0 -= (((DILITHIUM.PRIME-1)/2 - a0) >> 31) & DILITHIUM.PRIME;
		    a0 += (a0>>31)&DILITHIUM.PRIME;
		    return a0;
	    },

// ALPHA = (Q-1)/16
	    decompose_hi: function(params,a) {
		    var dv=params[2];
		    var a1;
		    a1  = (a + 127) >> 7;
		    if (dv==32) {
			    a1  = (a1*1025 + (1 << 21)) >> 22;
		    a1 &= 15;
		    } else {
			    a1  = (a1*11275 + (1 << 23)) >> 24;
			    a1 ^= ((43 - a1) >> 31) & a1;
		    }
		    return a1;
	    },

	    lobits: function(params,r0,r)
	    {
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
			    r0[m]=DILITHIUM.decompose_lo(params,r[m]);
	    },

	    hibits: function(params,r1,r)
	    {
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
			    r1[m]=DILITHIUM.decompose_hi(params,r[m]);
	    }, 

// before h initialised to zeros, hptr=0
// after new hptr returned and h[OMEGA+i]= hptr
	    MakePartialHint: function(params,h,hptr,z,r) {
		    var omega=params[7];
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
		    {
			    var a0=DILITHIUM.decompose_hi(params,r[m]);
			    var rz=r[m]+z[m];
			    rz-=DILITHIUM.PRIME;
			    rz=rz+((rz>>31)&DILITHIUM.PRIME);
			    var a1=DILITHIUM.decompose_hi(params,rz);
			    if (a0!=a1) {
				    if (hptr>=omega) return omega+1;
				    h[hptr++]=(m&0xff);
			    }
		    }
		    return hptr;
	    },

	    UsePartialHint: function(params,r,h,hptr,i,w) {
		    var dv=params[2];
		    var omega=params[7];
		    var md=(dv/2);
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
		    {
			    var a1=DILITHIUM.decompose_hi(params,w[m]);
			    if (m==(h[hptr]&0xff) && hptr<(h[omega+i]&0xff))
			    {
				    hptr++;
				    var a0=DILITHIUM.decompose_lo(params,w[m]);
				    if (a0<=DILITHIUM.PRIME/2) {
					    a1++;
					    if (a1>=md) a1-=md;
				    } else {
					    a1--;
					    if (a1<0) a1+=md;
				    }
			    }
			    r[m]=a1;
		    }
		    return hptr;
	    },

	    infinity_norm: function(w) {
		    var n=0;
		    for (var m=0;m<DILITHIUM.DEGREE;m++)
		    {
			    var az=w[m];
			    if (az>Math.floor(DILITHIUM.PRIME/2)) az=DILITHIUM.PRIME-az;
			    if (az>n) n=az;    
		    }
		    return n;
	    },

// Dilithium API
	    keypair: function(params,tau,sk,pk) {
            var buff=new Uint8Array(128);
            var rho=new Uint8Array(32);
            var rhod=new Uint8Array(64);
            var bK=new Uint8Array(32);
            var tr=new Uint8Array(32);
            var Aij=new Int32Array(DILITHIUM.DEGREE);
            var w=new Int32Array(DILITHIUM.DEGREE);
            var r=new Int32Array(DILITHIUM.DEGREE);

		    var ck=params[3];
		    var el=params[4];

            var s1 = new Array(el);
            for (var i = 0; i < el; i++) {
                s1[i] = new Uint8Array(DILITHIUM.DEGREE);
            }
            var s2 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                s2[i] = new Uint8Array(DILITHIUM.DEGREE);
            }
            var t0 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                t0[i] = new Int16Array(DILITHIUM.DEGREE);
            }
            var t1 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                t1[i] = new Int16Array(DILITHIUM.DEGREE);
            }

		    var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
  
		    for (var i=0;i<32;i++)
			    sh.process(tau[i]); 
		    sh.shake(buff,128);
		    for (var i=0;i<32;i++)
		    {
			    rho[i]=buff[i];
			    bK[i]=buff[i+96];
		    }
		    for (var i=0;i<64;i++)
			    rhod[i]=buff[32+i];

		    for (var i=0;i<el;i++)
			    DILITHIUM.sample_Sn(params,rhod,s1[i],i);

//console.log("Sn="+DILITHIUM.bytestostring(s1[0]));

		    for (var i=0;i<ck;i++)
		    {   
			    DILITHIUM.sample_Sn(params,rhod,s2[i],el+i);
			    DILITHIUM.poly_zero(r);
			    for (var j=0;j<el;j++)
			    {
//if (j==0) console.log("s1="+DILITHIUM.bytestostring(s1[j]));
				    DILITHIUM.poly_scopy(w,s1[j]);
//if (j==0) console.log("w1="+DILITHIUM.wordstostring(w));
				    DILITHIUM.ntt(w);  
//if (j==0) console.log("w2="+DILITHIUM.wordstostring(w));
				    DILITHIUM.ExpandAij(rho,Aij,i,j);  // This is bottleneck 
//if (j==0) console.log("Aij="+DILITHIUM.wordstostring(Aij));
				    DILITHIUM.poly_mul(w,w,Aij); 
				    DILITHIUM.poly_add(r,r,w);
			    }  
			    DILITHIUM.poly_hard_reduce(r); 
			    DILITHIUM.intt(r);

			    DILITHIUM.poly_scopy(w,s2[i]);
			    DILITHIUM.poly_pos(w);
			    DILITHIUM.poly_add(r,r,w);
			    DILITHIUM.poly_soft_reduce(r);
			    DILITHIUM.Power2Round(r,t0[i],t1[i]);
		    }

		    DILITHIUM.CRH1(params,tr,rho,t1);

		    var pklen=DILITHIUM.pack_pk(params,pk,rho,t1);
		    var sklen=DILITHIUM.pack_sk(params,sk,rho,bK,tr,s1,s2,t0);
	    },

        signature: function(params,sk,M,sig) {
            var badone;
            var rho = new Uint8Array(32);
            var bK = new Uint8Array(32);
            var ct = new Uint8Array(32);
            var tr = new Uint8Array(32);
            var mu = new Uint8Array(64);
            var rhod = new Uint8Array(64);
            var hint = new Uint8Array(100);

            var c = new Int32Array(DILITHIUM.DEGREE);  // 1024 bytes
            var w = new Int32Array(DILITHIUM.DEGREE);  // work space 1024 bytes
            var r = new Int32Array(DILITHIUM.DEGREE);  // work space 1024 bytes total= 21673 bytes
    //sign32 Aij[DL_DEGREE]; // 1024 bytes

            var ck=params[3];
            var el=params[4];

            var s1 = new Array(el);
            for (var i = 0; i < el; i++) {
                s1[i] = new Uint8Array(DILITHIUM.DEGREE);
            }
            var s2 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                s2[i] = new Uint8Array(DILITHIUM.DEGREE);
            }
            var t0 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                t0[i] = new Int16Array(DILITHIUM.DEGREE);
            }
            var y = new Array(el);
            for (var i = 0; i < el; i++) {
                y[i] = new Int32Array(DILITHIUM.DEGREE);
            }
            var Ay = new Array(ck);
            for (var i = 0; i < ck; i++) {
                Ay[i] = new Int32Array(DILITHIUM.DEGREE);
            }
            var w1 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                w1[i] = new Uint8Array(DILITHIUM.DEGREE);
            }

            var tau=params[0];
            var lg=params[1];
            var gamma1=(1<<lg);
            var dv=params[2];
            var gamma2=(DILITHIUM.PRIME-1)/dv;
            var eta=params[5];
            var beta=(tau*eta);
            var omega=params[7];

            DILITHIUM.unpack_sk(params,rho,bK,tr,s1,s2,t0,sk);
//console.log("tr= "+DILITHIUM.bytestostring(tr));
// signature
            DILITHIUM.CRH2(mu,tr,M,M.length);
//console.log("mu= "+DILITHIUM.bytestostring(mu));
            DILITHIUM.CRH3(rhod,bK,mu);
//console.log("rhod= "+DILITHIUM.bytestostring(rhod));
            for (var k=0; ;k++ )
            {
                var fk=k*el;
                DILITHIUM.sample_Y(params,fk,rhod,y);

                for (var i=0;i<el;i++)
                    DILITHIUM.ntt(y[i]);

                for (var i=0;i<ck;i++)
                {
                    DILITHIUM.poly_zero(r);
                    for (var j=0;j<el;j++)
                    { // Note: no NTTs in here
                        DILITHIUM.poly_copy(w,y[j]);
                        DILITHIUM.ExpandAij(rho,c,i,j);  // This is bottleneck // re-use c for Aij 
                        DILITHIUM.poly_mul(w,w,c);
                        DILITHIUM.poly_add(r,r,w);
                //poly_soft_reduce(r);  // be lazy
                    }
                    DILITHIUM.poly_hard_reduce(r);
                    DILITHIUM.intt(r);
                    DILITHIUM.poly_copy(Ay[i],r);
// Calculate w1
                    DILITHIUM.hibits(params,w1[i],Ay[i]);
                }

// Calculate c
                DILITHIUM.H4(params,ct,mu,w1);
                DILITHIUM.SampleInBall(params,ct,c);

                badone=false;
// Calculate z=y+c.s1
                DILITHIUM.ntt(c);
                for (var i=0;i<el;i++)
                {
                    DILITHIUM.poly_scopy(w,s1[i]);
                    DILITHIUM.ntt(w);
                    DILITHIUM.poly_mul(w,w,c);
                    DILITHIUM.nres_it(w);

                    DILITHIUM.poly_add(y[i],y[i],w);  // re-use y for z 
                    DILITHIUM.redc_it(y[i]);  // unNTT y
                    DILITHIUM.intt(y[i]);
                    DILITHIUM.poly_soft_reduce(y[i]);

                    if (DILITHIUM.infinity_norm(y[i])>=gamma1-beta)
                    {
                        badone=true;
                        break;
                    }
                } 
                if (badone) continue;
 
// Calculate Ay=w-c.s2 and r0=lobits(w-c.s2)
                var nh=0;
                for (var i=0;i<omega+ck;i++)
                    hint[i]=0;
                for (var i=0;i<ck;i++)
                {
                    DILITHIUM.poly_scopy(w,s2[i]);
                    DILITHIUM.ntt(w);
                    DILITHIUM.poly_mul(w,w,c);
            
                    DILITHIUM.intt(w);
                    DILITHIUM.poly_sub(Ay[i],Ay[i],w);
                    DILITHIUM.poly_soft_reduce(Ay[i]);   //Ay=w-cs2
                    DILITHIUM.lobits(params,w,Ay[i]);    // r0
                    if (DILITHIUM.infinity_norm(w)>=gamma2-beta)
                    {
                        badone=true;
                        break;
                    }
                    DILITHIUM.poly_copy(w,t0[i]);
                    DILITHIUM.ntt(w);
                    DILITHIUM.poly_mul(w,w,c);
            
                    DILITHIUM.intt(w);
                    DILITHIUM.poly_negate(r,w);  // -ct0
                    if (DILITHIUM.infinity_norm(r)>=gamma2)
                    {
                        badone=true;
                        break;
                    }
                    DILITHIUM.poly_sub(Ay[i],Ay[i],r);
                    DILITHIUM.poly_soft_reduce(Ay[i]);

                    nh=DILITHIUM.MakePartialHint(params,hint,nh,r,Ay[i]);
                    if (nh>omega)
                    {
                        badone=true;
                        break;
                    }
                    hint[omega+i]=nh;
                }
                if (badone) continue;
                break;
            }
//System.out.printf("y[0][0] = %d y[0][1] = %d\n",y[0][0],y[0][1]); 
            var siglen=DILITHIUM.pack_sig(params,sig,y,ct,hint);
            return k+1;      
        },

        verify: function(params,pk,M,sig) {
            var rho = new Uint8Array(32);
            var mu = new Uint8Array(64);
            var ct = new Uint8Array(32);
            var cct = new Uint8Array(32);
            var tr = new Uint8Array(32);
            var hint = new Uint8Array(100);

            var Aij = new Int32Array(DILITHIUM.DEGREE);  // 1024 bytes
            var c = new Int32Array(DILITHIUM.DEGREE);  // 1024 bytes
            var w = new Int32Array(DILITHIUM.DEGREE);  // 1024 bytes            
            var r = new Int32Array(DILITHIUM.DEGREE);  // 1024 bytes

            var ck=params[3];
            var el=params[4];

            var z = new Array(el);
            for (var i = 0; i < el; i++) {
                z[i] = new Int32Array(DILITHIUM.DEGREE);
            }
            var t1 = new Array(ck);
            for (var i = 0; i < ck; i++) {
                t1[i] = new Int16Array(DILITHIUM.DEGREE);
            }
            var w1d = new Array(ck);
            for (var i = 0; i < ck; i++) {
                w1d[i] = new Uint8Array(DILITHIUM.DEGREE);
            }

            var tau=params[0];
            var lg=params[1];
            var gamma1=(1<<lg);
            var eta=params[5];
            var beta=(tau*eta);
            var omega=params[7];

// unpack public key and signature
            DILITHIUM.unpack_pk(params,rho,t1,pk);
            DILITHIUM.unpack_sig(params,z,ct,hint,sig);

            for (var i=0;i<el;i++)
            {
                if (DILITHIUM.infinity_norm(z[i])>=gamma1-beta)
                    return false;
                DILITHIUM.ntt(z[i]); // convert to ntt form
            }

            DILITHIUM.CRH1(params,tr,rho,t1);
            DILITHIUM.CRH2(mu,tr,M,M.length);
            DILITHIUM.SampleInBall(params,ct,c);
            DILITHIUM.ntt(c);

// Calculate Az
            var hints=0;
            for (var i=0;i<ck;i++)
            {
                DILITHIUM.poly_zero(r);
                for (var j=0;j<el;j++)
                { // Note: no NTTs in here
                    DILITHIUM.poly_copy(w,z[j]);
                    DILITHIUM.ExpandAij(rho,Aij,i,j);    // This is bottleneck
                    DILITHIUM.poly_mul(w,w,Aij);
                    DILITHIUM.poly_add(r,r,w);
            //poly_soft_reduce(r);  // be lazy
                }
                DILITHIUM.poly_hard_reduce(r);

// Calculate Az-ct1.2^d
                for (var m=0;m<DILITHIUM.DEGREE;m++)
                    w[m]=(t1[i][m])<<DILITHIUM.D;
                DILITHIUM.ntt(w);
                DILITHIUM.poly_mul(w,w,c);
                DILITHIUM.poly_sub(r,r,w);
                DILITHIUM.intt(r);

                hints=DILITHIUM.UsePartialHint(params,w1d[i],hint,hints,i,r);
                if (hints>omega) return false;
            }

            DILITHIUM.H4(params,cct,mu,w1d);

            for (var i=0;i<32;i++)
                if (ct[i]!=cct[i])
                    return false;
            return true;
        },

	    keypair_2: function(tau,sk,pk)
	    {
		    DILITHIUM.keypair(DILITHIUM.PARAMS_2,tau,sk,pk);
	    },

        signature_2: function(sk,M,sig)
        {
            return DILITHIUM.signature(DILITHIUM.PARAMS_2,sk,M,sig);
        },

        verify_2: function(pk,M,sig)
        {
            return DILITHIUM.verify(DILITHIUM.PARAMS_2,pk,M,sig);
        },

	    keypair_3: function(tau,sk,pk)
	    {
		    DILITHIUM.keypair(DILITHIUM.PARAMS_3,tau,sk,pk);
	    },

        signature_3: function(sk,M,sig)
        {
            return DILITHIUM.signature(DILITHIUM.PARAMS_3,sk,M,sig);
        },

        verify_3: function(pk,M,sig)
        {
            return DILITHIUM.verify(DILITHIUM.PARAMS_3,pk,M,sig);
        },

	    keypair_5: function(tau,sk,pk)
	    {
		    DILITHIUM.keypair(DILITHIUM.PARAMS_5,tau,sk,pk);
	    },

        signature_5: function(sk,M,sig)
        {
            return DILITHIUM.signature(DILITHIUM.PARAMS_5,sk,M,sig);
        },

        verify_5: function(pk,M,sig)
        {
            return DILITHIUM.verify(DILITHIUM.PARAMS_5,pk,M,sig);
        },


        bytestostring: function(b) {
            var s = "",
                len = b.length,
                ch, i;

            for (i = 0; i < len; i++) {
                ch = b[i];
                s += ((ch >>> 4) & 15).toString(16);
                s += (ch & 15).toString(16);
            }

            return s;
        },

        wordstostring: function(b) {
            var s = "",
                len = b.length,
                ch, i;

            for (i = 0; i < len; i++) {
                ch = b[i];

                var s1= ((ch >>> 4) & 15).toString(16);
                s1+=(ch & 15).toString(16);
                ch >>>=8;
                var s2= ((ch >>> 4) & 15).toString(16);
                s2+=(ch & 15).toString(16);
                ch >>>=8;
                var s3= ((ch >>> 4) & 15).toString(16);
                s3+=(ch & 15).toString(16);
                ch >>>=8;
                var s4= ((ch >>> 4) & 15).toString(16);
                s4+=(ch & 15).toString(16);
                ch >>>=8;

                s+=s4+s3+s2+s1;
            }

            return s;
        },

        asciitobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i++) {
                b.push(s.charCodeAt(i));
            }

            return b;
        }
    };
    
//q= 8380417          
    DILITHIUM.PRIME = 0x7fe001; // q in Hex
    DILITHIUM.LGN = 8; // Degree n=2^LGN
    DILITHIUM.DEGREE = (1 << DILITHIUM.LGN);
    DILITHIUM.D = 13;
    DILITHIUM.TD = (23-DILITHIUM.D);

    DILITHIUM.NTTL = 1;

// Use 2^26 Montgomery modulus
    DILITHIUM.ONE = 0x3FFE00; // R mod Q
    DILITHIUM.COMBO = 0xA3FA;    // ONE/DEGREE mod Q
    DILITHIUM.R2MODP = 0x2419FF; // R^2 mod Q
    DILITHIUM.ND = 0xFC7FDFFF;  // 1/(R-Q) mod R

    DILITHIUM.MAXLG = 19;
    DILITHIUM.MAXK = 8;     // could reduce these if not using highest security
    DILITHIUM.MAXL = 7;
    DILITHIUM.YBYTES = (((DILITHIUM.MAXLG+1)*DILITHIUM.DEGREE)/8);

    DILITHIUM.SK_SIZE_2 = (32*3+DILITHIUM.DEGREE*(4*13+4*3+4*3)/8);
    DILITHIUM.PK_SIZE_2 = ((4*DILITHIUM.DEGREE*DILITHIUM.TD)/8+32);
    DILITHIUM.SIG_SIZE_2 = ((DILITHIUM.DEGREE*4*(17+1))/8+80+4+32);

    DILITHIUM.SK_SIZE_3 = (32*3+DILITHIUM.DEGREE*(6*13+5*4+6*4)/8);
    DILITHIUM.PK_SIZE_3 = ((6*DILITHIUM.DEGREE*DILITHIUM.TD)/8+32);
    DILITHIUM.SIG_SIZE_3 = ((DILITHIUM.DEGREE*5*(19+1))/8+55+6+32);

    DILITHIUM.SK_SIZE_5 = (32*3+DILITHIUM.DEGREE*(8*13+7*3+8*3)/8);
    DILITHIUM.PK_SIZE_5 = ((8*DILITHIUM.DEGREE*DILITHIUM.TD)/8+32);
    DILITHIUM.SIG_SIZE_5 = ((DILITHIUM.DEGREE*7*(19+1))/8+75+8+32);

	DILITHIUM.PARAMS_2 = [39,17,88,4,4,2,3,80];
	DILITHIUM.PARAMS_3 = [49,19,32,6,5,4,4,55];
	DILITHIUM.PARAMS_5 = [60,19,32,8,7,2,3,75];

    DILITHIUM.roots = [0x3ffe00,0x64f7,0x581103,0x77f504,0x39e44,0x740119,0x728129,0x71e24,0x1bde2b,0x23e92b,0x7a64ae,0x5ff480,0x2f9a75,0x53db0a,0x2f7a49,0x28e527,0x299658,0xfa070,0x6f65a5,0x36b788,0x777d91,0x6ecaa1,0x27f968,0x5fb37c,0x5f8dd7,0x44fae8,0x6a84f8,0x4ddc99,0x1ad035,0x7f9423,0x3d3201,0x445c5,0x294a67,0x17620,0x2ef4cd,0x35dec5,0x668504,0x49102d,0x5927d5,0x3bbeaf,0x44f586,0x516e7d,0x368a96,0x541e42,0x360400,0x7b4a4e,0x23d69c,0x77a55e,0x65f23e,0x66cad7,0x357e1e,0x458f5a,0x35843f,0x5f3618,0x67745d,0x38738c,0xc63a8,0x81b9a,0xe8f76,0x3b3853,0x3b8534,0x58dc31,0x1f9d54,0x552f2e,0x43e6e6,0x688c82,0x47c1d0,0x51781a,0x69b65e,0x3509ee,0x2135c7,0x67afbc,0x6caf76,0x1d9772,0x419073,0x709cf7,0x4f3281,0x4fb2af,0x4870e1,0x1efca,0x3410f2,0x70de86,0x20c638,0x296e9f,0x5297a4,0x47844c,0x799a6e,0x5a140a,0x75a283,0x6d2114,0x7f863c,0x6be9f8,0x7a0bde,0x1495d4,0x1c4563,0x6a0c63,0x4cdbea,0x40af0,0x7c417,0x2f4588,0xad00,0x6f16bf,0xdcd44,0x3c675a,0x470bcb,0x7fbe7f,0x193948,0x4e49c1,0x24756c,0x7ca7e0,0xb98a1,0x6bc809,0x2e46c,0x49a809,0x3036c2,0x639ff7,0x5b1c94,0x7d2ae1,0x141305,0x147792,0x139e25,0x67b0e1,0x737945,0x69e803,0x51cea3,0x44a79d,0x488058,0x3a97d9,0x1fea93,0x33ff5a,0x2358d4,0x3a41f8,0x4cdf73,0x223dfb,0x5a8ba0,0x498423,0x412f5,0x252587,0x6d04f1,0x359b5d,0x4a28a1,0x4682fd,0x6d9b57,0x4f25df,0xdbe5e,0x1c5e1a,0xde0e6,0xc7f5a,0x78f83,0x67428b,0x7f3705,0x77e6fd,0x75e022,0x503af7,0x1f0084,0x30ef86,0x49997e,0x77dcd7,0x742593,0x4901c3,0x53919,0x4610c,0x5aad42,0x3eb01b,0x3472e7,0x4ce03c,0x1a7cc7,0x31924,0x2b5ee5,0x291199,0x585a3b,0x134d71,0x3de11c,0x130984,0x25f051,0x185a46,0x466519,0x1314be,0x283891,0x49bb91,0x52308a,0x1c853f,0x1d0b4b,0x6fd6a7,0x6b88bf,0x12e11b,0x4d3e3f,0x6a0d30,0x78fde5,0x1406c7,0x327283,0x61ed6f,0x6c5954,0x1d4099,0x590579,0x6ae5ae,0x16e405,0xbdbe7,0x221de8,0x33f8cf,0x779935,0x54aa0d,0x665ff9,0x63b158,0x58711c,0x470c13,0x910d8,0x463e20,0x612659,0x251d8b,0x2573b7,0x7d5c90,0x1ddd98,0x336898,0x2d4bb,0x6d73a8,0x4f4cbf,0x27c1c,0x18aa08,0x2dfd71,0xc5ca5,0x19379a,0x478168,0x646c3e,0x51813d,0x35c539,0x3b0115,0x41dc0,0x21c4f7,0x70fbf5,0x1a35e7,0x7340e,0x795d46,0x1a4cd0,0x645caf,0x1d2668,0x666e99,0x6f0634,0x7be5db,0x455fdc,0x530765,0x5dc1b0,0x7973de,0x5cfd0a,0x2cc93,0x70f806,0x189c2a,0x49c5aa,0x776a51,0x3bcf2c,0x7f234f,0x6b16e0,0x3c15ca,0x155e68,0x72f6b7,0x1e29ce];
    DILITHIUM.iroots = [0x3ffe00,0x7f7b0a,0x7eafd,0x27cefe,0x78c1dd,0xd5ed8,0xbdee8,0x7c41bd,0x56fada,0x5065b8,0x2c04f7,0x50458c,0x1feb81,0x57b53,0x5bf6d6,0x6401d6,0x7b9a3c,0x42ae00,0x4bde,0x650fcc,0x320368,0x155b09,0x3ae519,0x20522a,0x202c85,0x57e699,0x111560,0x86270,0x492879,0x107a5c,0x703f91,0x5649a9,0x2ab0d3,0x6042ad,0x2703d0,0x445acd,0x44a7ae,0x71508b,0x77c467,0x737c59,0x476c75,0x186ba4,0x20a9e9,0x4a5bc2,0x3a50a7,0x4a61e3,0x19152a,0x19edc3,0x83aa3,0x5c0965,0x495b3,0x49dc01,0x2bc1bf,0x49556b,0x2e7184,0x3aea7b,0x442152,0x26b82c,0x36cfd4,0x195afd,0x4a013c,0x50eb34,0x7e69e1,0x56959a,0x454828,0x375fa9,0x3b3864,0x2e115e,0x15f7fe,0xc66bc,0x182f20,0x6c41dc,0x6b686f,0x6bccfc,0x2b520,0x24c36d,0x1c400a,0x4fa93f,0x3637f8,0x7cfb95,0x1417f8,0x744760,0x33821,0x5b6a95,0x319640,0x66a6b9,0x2182,0x38d436,0x4378a7,0x7212bd,0x10c942,0x7f3301,0x509a79,0x781bea,0x7bd511,0x330417,0x15d39e,0x639a9e,0x6b4a2d,0x5d423,0x13f609,0x59c5,0x12beed,0xa3d7e,0x25cbf7,0x64593,0x385bb5,0x2d485d,0x567162,0x5f19c9,0xf017b,0x4bcf0f,0x7df037,0x376f20,0x302d52,0x30ad80,0xf430a,0x3e4f8e,0x62488f,0x13308b,0x183045,0x5eaa3a,0x4ad613,0x1629a3,0x2e67e7,0x381e31,0x17537f,0x3bf91b,0x61b633,0xce94a,0x6a8199,0x43ca37,0x14c921,0xbcb2,0x4410d5,0x875b0,0x361a57,0x6743d7,0xee7fb,0x7d136e,0x22e2f7,0x66c23,0x221e51,0x2cd89c,0x3a8025,0x3fa26,0x10d9cd,0x197168,0x62b999,0x1b8352,0x659331,0x682bb,0x78abf3,0x65aa1a,0xee40c,0x5e1b0a,0x7bc241,0x44deec,0x4a1ac8,0x2e5ec4,0x1b73c3,0x385e99,0x66a867,0x73835c,0x51e290,0x6735f9,0x7d63e5,0x309342,0x126c59,0x7d0b46,0x4c7769,0x620269,0x28371,0x5a6c4a,0x5ac276,0x1eb9a8,0x39a1e1,0x76cf29,0x38d3ee,0x276ee5,0x1c2ea9,0x198008,0x2b35f4,0x846cc,0x4be732,0x5dc219,0x74041a,0x68fbfc,0x14fa53,0x26da88,0x629f68,0x1386ad,0x1df292,0x4d6d7e,0x6bd93a,0x6e21c,0x15d2d1,0x32a1c2,0x6cfee6,0x145742,0x10095a,0x62d4b6,0x635ac2,0x2daf77,0x362470,0x57a770,0x6ccb43,0x397ae8,0x6785bb,0x59efb0,0x6cd67d,0x41fee5,0x6c9290,0x2785c6,0x56ce68,0x54811c,0x7cc6dd,0x65633a,0x32ffc5,0x4b6d1a,0x412fe6,0x2532bf,0x7b7ef5,0x7aa6e8,0x36de3e,0xbba6e,0x8032a,0x364683,0x4ef07b,0x60df7d,0x2fa50a,0x9ffdf,0x7f904,0xa8fc,0x189d76,0x78507e,0x7360a7,0x71ff1b,0x6381e7,0x7221a3,0x30ba22,0x1244aa,0x395d04,0x35b760,0x4a44a4,0x12db10,0x5aba7a,0x7bcd0c,0x365bde,0x255461,0x5da206,0x33008e,0x459e09,0x5c872d,0x4be0a7,0x5ff56e];
    return DILITHIUM;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports.DILITHIUM = DILITHIUM;
}

