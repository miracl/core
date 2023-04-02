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

var KYBER = function(ctx) {
    "use strict";

    var KYBER = {

        montgomery_reduce: function(a) {
            //console.log("a= "+a);
            var t = (a*KYBER.QINV)&0xffff;
            //console.log("mt1= "+t);
            t = ((a - t*KYBER.PRIME)>>16);
            //console.log("mt2= "+t);
            return t;
        },

        barrett_reduce: function(a) {
            //console.log("ba= "+a);
            var v = Math.floor(((1<<26) + (KYBER.PRIME>>1))/KYBER.PRIME);
            //console.log("bv= "+v);
            var t  = ((v*a + 0x2000000)>>26);
            t *= KYBER.PRIME;
            //console.log("bt= "+((a - t)&0xffff));
            return (a - t)&0xffff;
        },

        fqmul: function(a, b) {
            return KYBER.montgomery_reduce(a*b);
        },

        ntt: function(r) {
            var k = 1;
            for (var len = 128; len >= 2; len >>= 1) {
                for (var start = 0; start < 256; start = j + len) {
                    var zeta = KYBER.zetas[k++];
                    for(var j = start; j < start + len; j++) {
                        var t = KYBER.fqmul(zeta, r[j + len]);
                        r[j + len] = (r[j] - t);
                        r[j] = (r[j] + t);
                    }
                }
            }
        },

        invntt: function(r) {
            var start;
            var f = 1441; // mont^2/128

            var k = 127;
            for(var len = 2; len <= 128; len <<= 1) {
                for(start = 0; start < 256; start = j + len) {
                    var zeta = KYBER.zetas[k--];
                    for(var j = start; j < start + len; j++) {
                        var t = r[j];
                        r[j] = KYBER.barrett_reduce((t + r[j + len]));
                        r[j + len] = (r[j + len] - t);
                        r[j + len] = KYBER.fqmul(zeta, r[j + len]);
                    }
                }
            }

            for(j = 0; j < 256; j++)
                r[j] = KYBER.fqmul(r[j], f);
        },

        basemul: function(index,r,a,b,zeta) {
            var i=index;
            var j=index+1;
            r[i]  = KYBER.fqmul(a[j], b[j]);
            r[i]  = KYBER.fqmul(r[i], zeta);
            r[i] += KYBER.fqmul(a[i], b[i]);
            r[j]  = KYBER.fqmul(a[i], b[j]);
            r[j] += KYBER.fqmul(a[j], b[i]);
        },

        poly_reduce: function(r) {
            for(var i=0;i<KYBER.DEGREE;i++)
            r[i] = KYBER.barrett_reduce(r[i]);
        },

        poly_ntt: function(r) {
            KYBER.ntt(r);
            KYBER.poly_reduce(r);
        },

        poly_invntt: function(r)
        {
            KYBER.invntt(r);
        },

// Note r must be distinct from a and b
        poly_mul: function(r, a, b)
        {
            for(var i = 0; i < KYBER.DEGREE/4; i++) {
                KYBER.basemul(4*i,r,a,b,KYBER.zetas[64+i]);
                KYBER.basemul(4*i+2,r,a,b,-KYBER.zetas[64+i]);
            }
        },

        poly_tomont: function(r)
        {
            var f = KYBER.ONE;
            for(var i=0;i<KYBER.DEGREE;i++)
                r[i] = KYBER.montgomery_reduce(r[i]*f);
        },

/* End of public domain reference code use */

// copy polynomial
        poly_copy: function(p1,p2) {
            for (var i = 0; i < KYBER.DEGREE; i++)
                p1[i] = p2[i];
        },

// zero polynomial
        poly_zero: function(p1) {
            for (var i = 0; i < KYBER.DEGREE; i++)
                p1[i] = 0;
        },

// add polynomials
        poly_add: function(p1,p2, p3) {
            for (var i = 0; i < KYBER.DEGREE; i++)
                p1[i] = (p2[i] + p3[i]);
        },

// subtract polynomials
        poly_sub: function(p1,p2,p3) {
            for (var i = 0; i < KYBER.DEGREE; i++)
                p1[i] = (p2[i] - p3[i]);
        },

// Generate A[i][j] from rho
        ExpandAij: function(rho,Aij,i,j) {
            var sh = new ctx.SHA3(ctx.SHA3.SHAKE128);
            var buff = new Uint8Array(640);
            for (var m=0;m<32;m++)
                sh.process(rho[m]);

            sh.process(j&0xff);
            sh.process(i&0xff);
            sh.shake(buff,640);
            var i=0;
            var j=0;
            while (j<KYBER.DEGREE)
            {
                var t1=(buff[i])&0xff;  // convert byte to unsigned
                var t2=(buff[i+1])&0xff;
                var t3=(buff[i+2])&0xff;
                var d1=(t1+256*(t2&0x0f));
                var d2=(Math.floor(t2/16)+16*t3);
                if (d1<KYBER.PRIME)
                    Aij[j++]=d1;
                if (d2<KYBER.PRIME && j<KYBER.DEGREE)
                    Aij[j++]=d2;
                i+=3;
            }
        },

// get n-th bit from byte array
        getbit: function(b,n) {
            var wd=Math.floor(n/8);
            var bt=n%8;
            return (b[wd]>>bt)&1;
        },

// centered binomial distribution
        CBD: function(bts,eta,f) {
            for (var i=0;i<KYBER.DEGREE;i++)
            {
                var a=0; var b=0;
                for (var j=0;j<eta;j++)
                {
                    a+=KYBER.getbit(bts,2*i*eta+j);
                    b+=KYBER.getbit(bts,2*i*eta+eta+j);
                }
                f[i]=(a-b); 
            }
        },

        caste: function(x) {
            return (x&0xff);
        },

// extract ab bits into word from dense byte stream
        nextword: function(ab,t,position) {
            var ptr=position[0]; // index in array
            var bts=position[1]; // bit index in byte
            var r=(     (t[ptr]&0xff)    >>bts);
            var mask=((1<<ab)-1);
            var i=0;

            var gotbits=8-bts; // bits left in current byte
            while (gotbits<ab)
            {
                i++;
                var w=((t[ptr+i])&0xff);
                r|=w<<gotbits;
                gotbits+=8;
            }
            bts+=ab;
            while (bts>=8)
            {
                bts-=8;
                ptr++;
            }
            var w=(r&mask);
            position[0]=ptr;
            position[1]=bts;
            return w;  
        },

// array t has ab active bits per word
// extract bytes from array of words
// if max!=0 then -max<=t[i]<=+max
        nextbyte16: function(ab,t,position) {
            var ptr=position[0]; // index in array
            var bts=position[1]; // bit index in word
            var left=ab-bts; // number of bits left in this word
            var k=ptr%256;

            var w=t[k]; w+=(w>>15)&KYBER.PRIME;
            var r=(w>>bts);
            var i=0;
            while (left<8)
            {
                i++;
                var w=t[k+i]; w+=(w>>15)&KYBER.PRIME;
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

// encode polynomial vector with coefficients of length L, into packed bytes
// pos indicates current position in vector t
// pptr indicates which vector in the matrix
        encode: function(t,pos,L,pack,pptr) {
            var k=(KYBER.DEGREE*L)/8;  // compressed length
            for (var n=0;n<k;n++ ) {
                pack[n+pptr*k]=KYBER.nextbyte16(L,t,pos);
            }
        },

        chk_encode: function(t,pos,L,pack,pptr) {
            var k=(KYBER.DEGREE*L)/8;
            var m,diff=0;
            for (var n=0;n<k;n++ ) {
                m=KYBER.nextbyte16(L,t,pos);
                diff|=(m^pack[n+pptr*k]);
            }    
            return diff;
        },

// decode packed bytes into polynomial vector, with coefficients of length L
// pos indicates current position in byte array pack
        decode: function(pack,L,t,pos) {
            for (var i=0;i<KYBER.DEGREE;i++ )
                t[i]=KYBER.nextword(L,pack,pos);
        },

// compress polynomial coefficents in place, for polynomial vector of length len
        compress: function(t,d) {
            var twod=(1<<d);
            for (var i=0;i<KYBER.DEGREE;i++)
            {
                t[i]+=(t[i]>>15)&KYBER.PRIME;
                t[i]= (    Math.floor((twod*t[i]+(KYBER.PRIME>>1))/KYBER.PRIME)    &(twod-1));
            }
        },

// decompress polynomial coefficents in place, for polynomial vector of length len
        decompress: function(t,d) {
            var twod1=(1<<(d-1));
            for (var i=0;i<KYBER.DEGREE;i++)
                t[i]=((KYBER.PRIME*t[i]+twod1)>>d);
        },

// input entropy, output key pair
        CPA_keypair: function(params,tau,sk,pk) {
            var sh = new ctx.SHA3(ctx.SHA3.HASH512);
            var rho = new Uint8Array(32); 
            var sigma = new Uint8Array(33); 
            var buff = new Uint8Array(256); 

            var ck=params[0];
            var r = new Int16Array(KYBER.DEGREE);
            var w = new Int16Array(KYBER.DEGREE);
            var Aij = new Int16Array(KYBER.DEGREE);

            var s = new Array(ck);
            for (var i = 0; i < ck; i++) {
                s[i] = new Int16Array(KYBER.DEGREE);
            }
            var e = new Array(ck);
            for (var i = 0; i < ck; i++) {
                e[i] = new Int16Array(KYBER.DEGREE);
            }
            var p = new Array(ck);
            for (var i = 0; i < ck; i++) {
                p[i] = new Int16Array(KYBER.DEGREE);
            }

            var eta1=params[1];
            var public_key_size=32+ck*(KYBER.DEGREE*3)/2;
            var secret_cpa_key_size=ck*(KYBER.DEGREE*3)/2;
   
            for (var i=0;i<32;i++)
                sh.process(tau[i]); 
            var bf = sh.hash();

//console.log("bf = "+bf[6]);

            for (var i=0;i<32;i++)
            {
                rho[i]=bf[i];
                sigma[i]=bf[i+32];
            }
            sigma[32]=0;   // N

// create s
            for (var i=0;i<ck;i++)
            {
                var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var j=0;j<33;j++)
                    sh.process(sigma[j]); 
                sh.shake(buff,64*eta1);
                KYBER.CBD(buff,eta1,s[i]);
                sigma[32]+=1;
            }
//console.log("s = "+s[0][7]+" "+s[0][8]+" "+s[0][9]+" "+s[0][10]);
// create e
            for (var i=0;i<ck;i++)
            {
                var sh=new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var j=0;j<33;j++)
                    sh.process(sigma[j]); 
                sh.shake(buff,64*eta1);
                KYBER.CBD(buff,eta1,e[i]);
                sigma[32]+=1;
            }

            for (var k=0;k<ck;k++)
            {
                KYBER.poly_ntt(s[k]);
                KYBER.poly_ntt(e[k]);
            }

            for (var i=0;i<ck;i++)
            {
                KYBER.ExpandAij(rho,Aij,i,0);
                KYBER.poly_mul(r,Aij,s[0]);

                for (var j=1;j<ck;j++)
                {
                    KYBER.ExpandAij(rho,Aij,i,j);
                    KYBER.poly_mul(w,s[j],Aij);
                    KYBER.poly_add(r,r,w);
                }
                KYBER.poly_reduce(r);
                KYBER.poly_tomont(r);
                KYBER.poly_add(p[i],r,e[i]);
                KYBER.poly_reduce(p[i]);
            }

            var pos=[];
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.encode(s[i],pos,12,sk,i);
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.encode(p[i],pos,12,pk,i);
            for (var i=0;i<32;i++)
                pk[public_key_size-32+i]=rho[i];
        },

// input 64 random bytes, output secret and public keys
        CCA_keypair: function(params,randbytes64,sk,pk) {
            var sh = new ctx.SHA3(ctx.SHA3.HASH256);
            var sks=(params[0]*(KYBER.DEGREE*3)/2);
            var pks=(32+params[0]*(KYBER.DEGREE*3)/2);

            KYBER.CPA_keypair(params,randbytes64,sk,pk);
            for (var i=0;i<pks;i++)
                sk[sks+i]=pk[i];

            for (var i=0;i<pks;i++)
                sh.process(pk[i]);
            var h=sh.hash();
            for (var i=0;i<32;i++)
                sk[sks+pks+i]=h[i];
            for (var i=0;i<32;i++)
                sk[sks+pks+32+i]=randbytes64[32+i];
        },

        CPA_base_encrypt: function(params,coins,pk,ss,u,v) {
            var sigma = new Uint8Array(33); 
            var buff = new Uint8Array(256); 
            var rho = new Uint8Array(32); 

            var ck=params[0];
            var r = new Int16Array(KYBER.DEGREE);
            var w = new Int16Array(KYBER.DEGREE);
            var Aij = new Int16Array(KYBER.DEGREE);

            var q = new Array(ck);
            for (var i = 0; i < ck; i++) {
                q[i] = new Int16Array(KYBER.DEGREE);
            }
            var p = new Array(ck);
            for (var i = 0; i < ck; i++) {
                p[i] = new Int16Array(KYBER.DEGREE);
            }

            var eta1=params[1];
            var eta2=params[2];
            var du=params[3];
            var dv=params[4];
            var public_key_size=32+ck*(KYBER.DEGREE*3)/2;

            for (var i=0;i<32;i++)
                sigma[i]=coins[i];//i+6; //RAND_byte(RNG);
            sigma[32]=0;

            for (var i=0;i<32;i++)
                rho[i]=pk[public_key_size-32+i];

// create q
            for (var i=0;i<ck;i++)
            {
                var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var j=0;j<33;j++)
                    sh.process(sigma[j]); 
                sh.shake(buff,64*eta1);
                KYBER.CBD(buff,eta1,q[i]);
                sigma[32]+=1;
            }
// create e1
            for (var i=0;i<ck;i++)
            {
                var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
                for (var j=0;j<33;j++)
                    sh.process(sigma[j]); 
                sh.shake(buff,64*eta2);
                KYBER.CBD(buff,eta1,u[i]);          // e1
                sigma[32]+=1;
            }
            for (var i=0;i<ck;i++)
                KYBER.poly_ntt(q[i]);
    
            for (var i=0;i<ck;i++)
            {
                KYBER.ExpandAij(rho,Aij,0,i);
                KYBER.poly_mul(r,Aij,q[0]);
                for (var j=1;j<ck;j++)
                {
                    KYBER.ExpandAij(rho,Aij,j,i);
                    KYBER.poly_mul(w,q[j],Aij);
                    KYBER.poly_add(r,r,w);
                }
                KYBER.poly_reduce(r);
                KYBER.poly_invntt(r);
                KYBER.poly_add(u[i],u[i],r);
                KYBER.poly_reduce(u[i]);
            }

            var pos=[];
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.decode(pk,12,p[i],pos);    
    
            KYBER.poly_mul(v,p[0],q[0]);

            for (var i=1;i<ck;i++)
            {
                KYBER.poly_mul(r,p[i],q[i]);
                KYBER.poly_add(v,v,r);
            }    
            KYBER.poly_invntt(v);        

// create e2
            var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
            for (var j=0;j<33;j++)
                sh.process(sigma[j]); 
            sh.shake(buff,64*eta2);
            KYBER.CBD(buff,eta1,w);  // e2

            KYBER.poly_add(v,v,w);
            pos[0]=pos[1]=0;
            KYBER.decode(ss,1,r,pos);
            KYBER.decompress(r,1);

            KYBER.poly_add(v,v,r);
            KYBER.poly_reduce(v);

            for (var i=0;i<ck;i++)
                KYBER.compress(u[i],du);
            KYBER.compress(v,dv);        
        },

// Given input of entropy, public key and shared secret is an input, outputs ciphertext
        CPA_encrypt: function(params,coins,pk,ss,ct) {
            var ck=params[0];
            var v = new Int16Array(KYBER.DEGREE);
            var u = new Array(ck);
            for (var i = 0; i < ck; i++) {
                u[i] = new Int16Array(KYBER.DEGREE);
            }

            var du=params[3];
            var dv=params[4];
            var ciphertext_size=(du*ck+dv)*KYBER.DEGREE/8;

            KYBER.CPA_base_encrypt(params,coins,pk,ss,u,v);
            var pos=[];
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.encode(u[i],pos,du,ct,i);

            var len=(dv*KYBER.DEGREE/8); // last part of CT
            var last = new Uint8Array(len); 

            KYBER.encode(v,pos,dv,last,0);
            for (var i=0;i<len;i++)
                ct[ciphertext_size-len+i]=last[i];
        },

// Re-encrypt and check that ct is OK (if so return is zero)
        CPA_check_encrypt: function(params,coins,pk,ss,ct) {
            var ck=params[0];
            var v = new Int16Array(KYBER.DEGREE);
            var u = new Array(ck);
            for (var i = 0; i < ck; i++) {
                u[i] = new Int16Array(KYBER.DEGREE);
            }

            var du=params[3];
            var dv=params[4];
            var ciphertext_size=(du*ck+dv)*KYBER.DEGREE/8;
        
            KYBER.CPA_base_encrypt(params,coins,pk,ss,u,v);
            var pos=[];
            pos[0]=pos[1]=0;
            var d1=0;
            for (var i=0;i<ck;i++)
            d1|=KYBER.chk_encode(u[i],pos,du,ct,i);

            var len=(dv*KYBER.DEGREE/8); // last part of CT
            var last = new Uint8Array(len);
            for (var i=0;i<len;i++)
                last[i]=ct[ciphertext_size-len+i];

            var d2=KYBER.chk_encode(v,pos,dv,last,0);

            if ((d1|d2)==0)
                return 0;
            else
                return 0xff;
        },

        CCA_encrypt: function(params,randbytes32,pk,ss,ct) {
            var coins = new Uint8Array(32);

            var ck=params[0];
            var du=params[3];
            var dv=params[4];
            var shared_secret_size=params[5];
            var public_key_size=32+ck*(KYBER.DEGREE*3)/2;
            var ciphertext_size=(du*ck+dv)*KYBER.DEGREE/8;

            var sh = new ctx.SHA3(ctx.SHA3.HASH256);
            for (var i=0;i<32;i++)
                sh.process(randbytes32[i]);
            var hm= sh.hash();

            sh = new ctx.SHA3(ctx.SHA3.HASH256);
            for (var i=0;i<public_key_size;i++)
                sh.process(pk[i]);
            var h= sh.hash();

            sh = new ctx.SHA3(ctx.SHA3.HASH512);
            for (var i=0;i<32;i++)
                sh.process(hm[i]);
            for (var i=0;i<32;i++)
                sh.process(h[i]);
            var g= sh.hash();

            for (var i=0;i<32;i++)
                coins[i]=g[i+32];

            KYBER.CPA_encrypt(params,coins,pk,hm,ct);

            sh = new ctx.SHA3(ctx.SHA3.HASH256);
            for (var i=0;i<ciphertext_size;i++)
                sh.process(ct[i]);
            h= sh.hash();

            sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
            for (var i=0;i<32;i++)
                sh.process(g[i]);
            for (var i=0;i<32;i++)
                sh.process(h[i]);

            sh.shake(ss,shared_secret_size);
        },

        CPA_decrypt: function(params,SK,CT,SS) {
            var ck=params[0];
            var w=new Int16Array(KYBER.DEGREE);
            var v=new Int16Array(KYBER.DEGREE);
            var r=new Int16Array(KYBER.DEGREE);

            var u = new Array(ck);
            for (var i = 0; i < ck; i++) {
                u[i] = new Int16Array(KYBER.DEGREE);
            }
            var s = new Array(ck);
            for (var i = 0; i < ck; i++) {
                s[i] = new Int16Array(KYBER.DEGREE);
            }

            var du=params[3];
            var dv=params[4];
            var shared_secret_size=params[5];

            var pos=[];
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.decode(CT,du,u[i],pos);
            KYBER.decode(CT,dv,v,pos);
            for (var i=0;i<ck;i++)
                KYBER.decompress(u[i],du);
            KYBER.decompress(v,dv);
            pos[0]=pos[1]=0;
            for (var i=0;i<ck;i++)
                KYBER.decode(SK,12,s[i],pos);

            KYBER.poly_ntt(u[0]);
            KYBER.poly_mul(w,u[0],s[0]);
            for (var i=1;i<ck;i++)
            {
                KYBER.poly_ntt(u[i]);
                KYBER.poly_mul(r,u[i],s[i]);
                KYBER.poly_add(w,w,r);
            }
            KYBER.poly_reduce(w);
            KYBER.poly_invntt(w);
            KYBER.poly_sub(v,v,w);
            KYBER.compress(v,1);
            pos[0]=pos[1]=0;
            KYBER.encode(v,pos,1,SS,0); 
        },

        CCA_decrypt: function(params,SK,CT,SS) {
            var ck=params[0];
            var du=params[3];
            var dv=params[4];
            var secret_cpa_key_size=ck*(KYBER.DEGREE*3)/2;
            var public_key_size=32+ck*(KYBER.DEGREE*3)/2;
            var shared_secret_size=params[5];
            var ciphertext_size=(du*ck+dv)*KYBER.DEGREE/8;

            var h=new Uint8Array(32);
            var z=new Uint8Array(32);
            var m=new Uint8Array(32);
            var coins=new Uint8Array(32);
            var PK=new Uint8Array(public_key_size);

            for (var i=0;i<public_key_size;i++)
                PK[i]=SK[secret_cpa_key_size+i];

            for (var i=0;i<32;i++)
                h[i]=SK[secret_cpa_key_size+public_key_size+i];
            for (var i=0;i<32;i++)
                z[i]=SK[secret_cpa_key_size+public_key_size+32+i];

            KYBER.CPA_decrypt(params,SK,CT,m);

            var sh = new ctx.SHA3(ctx.SHA3.HASH512);
            for (var i=0;i<32;i++)
                sh.process(m[i]);
            for (var i=0;i<32;i++)
                sh.process(h[i]);
            var g= sh.hash();

            for (var i=0;i<32;i++)
                coins[i]=g[i+32];

            var mask=KYBER.CPA_check_encrypt(params,coins,PK,m,CT);
            for (var i=0;i<32;i++)
                g[i]^=(g[i]^z[i])&mask;               // substitute z for Kb on failure

            sh = new ctx.SHA3(ctx.SHA3.HASH256);
            for (var i=0;i<ciphertext_size;i++)
                sh.process(CT[i]);
            h=sh.hash();

            sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
            for (var i=0;i<32;i++)
                sh.process(g[i]);
            for (var i=0;i<32;i++)
                sh.process(h[i]);
            sh.shake(SS,shared_secret_size);
        },

        keypair512: function(r64,SK,PK) {
            KYBER.CCA_keypair(KYBER.PARAMS_512,r64,SK,PK);
        },

        encrypt512: function(r32,PK,SS,CT) {
            KYBER.CCA_encrypt(KYBER.PARAMS_512,r32,PK,SS,CT);
        },

        decrypt512: function(SK,CT,SS) {
            KYBER.CCA_decrypt(KYBER.PARAMS_512,SK,CT,SS);
        },

        keypair768: function(r64,SK,PK) {
            KYBER.CCA_keypair(KYBER.PARAMS_768,r64,SK,PK);
        },

        encrypt768: function(r32,PK,SS,CT) {
            KYBER.CCA_encrypt(KYBER.PARAMS_768,r32,PK,SS,CT);
        },

        decrypt768: function(SK,CT,SS) {
            KYBER.CCA_decrypt(KYBER.PARAMS_768,SK,CT,SS);
        },

        keypair1024: function(r64,SK,PK) {
            KYBER.CCA_keypair(KYBER.PARAMS_1024,r64,SK,PK);
        },

        encrypt1024: function(r32,PK,SS,CT) {
            KYBER.CCA_encrypt(KYBER.PARAMS_1024,r32,PK,SS,CT);
        },

        decrypt1024: function(SK,CT,SS) {
            KYBER.CCA_decrypt(KYBER.PARAMS_1024,SK,CT,SS);
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
        }

    };
    //q=12289
            
    KYBER.PRIME = 0xD01; // q in Hex
    KYBER.LGN = 8; // Degree n=2^LGN
    KYBER.ONE = 0x549; // R mod q
    KYBER.QINV = 62209;
    KYBER.DEGREE = 256; // 1<< LGN
    KYBER.MODINV = Math.pow(2, -26);

    KYBER.PARAMS_512 = [2,3,2,10,4,32];
    KYBER.PARAMS_768 = [3,2,2,10,4,32];
    KYBER.PARAMS_1024 = [4,2,2,11,5,32];

    KYBER.SECRET_CPA_SIZE_512 = (2*(KYBER.DEGREE*3)/2);
    KYBER.PUBLIC_SIZE_512 = (32+2*(KYBER.DEGREE*3)/2);
    KYBER.CIPHERTEXT_SIZE_512 = ((10*2+4)*KYBER.DEGREE/8);
    KYBER.SECRET_CCA_SIZE_512 = (KYBER.SECRET_CPA_SIZE_512+KYBER.PUBLIC_SIZE_512+64);
    KYBER.SHARED_SECRET_512 = 32;

    KYBER.SECRET_CPA_SIZE_768 = (3*(KYBER.DEGREE*3)/2);
    KYBER.PUBLIC_SIZE_768 = (32+3*(KYBER.DEGREE*3)/2);
    KYBER.CIPHERTEXT_SIZE_768 = ((10*3+4)*KYBER.DEGREE/8);
    KYBER.SECRET_CCA_SIZE_768 = (KYBER.SECRET_CPA_SIZE_768+KYBER.PUBLIC_SIZE_768+64);
    KYBER.SHARED_SECRET_768 = 32;

    KYBER.SECRET_CPA_SIZE_1024 = (4*(KYBER.DEGREE*3)/2);
    KYBER.PUBLIC_SIZE_1024 = (32+4*(KYBER.DEGREE*3)/2);
    KYBER.CIPHERTEXT_SIZE_1024 = ((11*4+5)*KYBER.DEGREE/8);
    KYBER.SECRET_CCA_SIZE_1024 = (KYBER.SECRET_CPA_SIZE_1024+KYBER.PUBLIC_SIZE_1024+64);
    KYBER.SHARED_SECRET_1024 = 32;

    KYBER.MAXK = 4;

    KYBER.zetas = [-1044,  -758,  -359, -1517,  1493,  1422,   287,   202,
       -171,   622,  1577,   182,   962, -1202, -1474,  1468,
        573, -1325,   264,   383,  -829,  1458, -1602,  -130,
       -681,  1017,   732,   608, -1542,   411,  -205, -1571,
       1223,   652,  -552,  1015, -1293,  1491,  -282, -1544,
        516,    -8,  -320,  -666, -1618, -1162,   126,  1469,
       -853,   -90,  -271,   830,   107, -1421,  -247,  -951,
       -398,   961, -1508,  -725,   448, -1065,   677, -1275,
      -1103,   430,   555,   843, -1251,   871,  1550,   105,
        422,   587,   177,  -235,  -291,  -460,  1574,  1653,
       -246,   778,  1159,  -147,  -777,  1483,  -602,  1119,
      -1590,   644,  -872,   349,   418,   329,  -156,   -75,
        817,  1097,   603,   610,  1322, -1285, -1465,   384,
      -1215,  -136,  1218, -1335,  -874,   220, -1187, -1659,
      -1185, -1530, -1278,   794, -1510,  -854,  -870,   478,
       -108,  -308,   996,   991,   958, -1460,  1522,  1628];

    return KYBER;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports.KYBER = KYBER;
}

