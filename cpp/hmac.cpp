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
    HMAC functions
*/

#include "arch.h"
#include "core.h"

using namespace core;

#define ROUNDUP(a,b) ((a)-1)/(b)+1
#define CEIL(a,b) (((a)-1)/(b)+1)

/* General Purpose hash function, padding with zeros, optional input octets p and x, optional integer n,hash to octet w of length olen */
/* hash is the Hash family, either MC_SHA2 or MC_SHA3 */
/* hlen should be 32,48 or 64 for MC_SHA2 (that is SHA256/384/512) */
/* hlen should be 24,32,48,64 for MC_SHA3 */
/* olen=0 -     output = hlen bytes */
/* olen<=hlen -  output = olen bytes */
/* olen>hlen  -  output is padded with leading zeros and then hlen bytes */

void core::GPhash(int hash,int hlen,octet *w,int olen,int pad,octet *p,int n,octet *x)
{
    hash256 sh256;
    hash384 sh384;
    hash512 sh512;
    sha3 sh3;
    int i,c[4];
    char hh[64];

    if (n>=0)
    {
        c[0] = (n >> 24) & 0xff;
        c[1] = (n >> 16) & 0xff;
        c[2] = (n >> 8) & 0xff;
        c[3] = (n) & 0xff;
    }

    switch (hash)
    {
    case MC_SHA2 :
        switch (hlen)
        {
        case SHA256 :
            HASH256_init(&sh256);
            for (i=0;i<pad;i++) HASH256_process(&sh256,0);
            if (p!=NULL)
                for (i=0;i<p->len;i++) HASH256_process(&sh256,p->val[i]);
            if (n>=0)
                for (i=0;i<4;i++) HASH256_process(&sh256,c[i]);
            if (x!=NULL)
                for (i=0;i<x->len;i++) HASH256_process(&sh256,x->val[i]);
            HASH256_hash(&sh256,hh);
            break;
        case SHA384 :
            HASH384_init(&sh384);
            for (i=0;i<pad;i++) HASH384_process(&sh384,0);
            if (p!=NULL)
                for (i=0;i<p->len;i++) HASH384_process(&sh384,p->val[i]);
            if (n>=0)
                for (i=0;i<4;i++) HASH384_process(&sh384,c[i]);
            if (x!=NULL)
                for (i=0;i<x->len;i++) HASH384_process(&sh384,x->val[i]);
            HASH384_hash(&sh384,hh);
            break;
        case SHA512 :
            HASH512_init(&sh512);
            for (i=0;i<pad;i++) HASH512_process(&sh512,0);
            if (p!=NULL)
                for (i=0;i<p->len;i++) HASH512_process(&sh512,p->val[i]);
            if (n>=0)
                for (i=0;i<4;i++) HASH512_process(&sh512,c[i]);
            if (x!=NULL)
                for (i=0;i<x->len;i++) HASH512_process(&sh512,x->val[i]);
            HASH512_hash(&sh512,hh);   
            break;
        }
        break;
    case MC_SHA3 :
        SHA3_init(&sh3,hlen);
        for (i=0;i<pad;i++) SHA3_process(&sh3,0);
        if (p!=NULL)
            for (i=0;p->len;i++) SHA3_process(&sh3,p->val[i]);
        if (n>=0)
            for (i=0;i<4;i++) SHA3_process(&sh3,c[i]);
        if (x!=NULL)
            for (i=0;x->len;i++) SHA3_process(&sh3,x->val[i]);
        SHA3_hash(&sh3,hh);  
        break;
    default: return;
    }
    OCT_empty(w);
    if (!olen)
        OCT_jbytes(w,hh,hlen);
    else
    {
        if (olen<=hlen)
        {
            OCT_jbytes(w,hh,olen);
        } else {
            OCT_jbyte(w, 0, olen - hlen);
            OCT_jbytes(w, hh, hlen);
        }
    }
}

/* Simple hash octet p to octet w of length hlen */
void core::SPhash(int hash, int hlen,octet *w, octet *p)
{
    GPhash(hash, hlen, w, 0, 0, p, -1, NULL);
}

static int blksize(int hash,int hlen)
{
    int blk=0;
    switch (hash)
    {
    case MC_SHA2 :
            blk=64;
            if (hlen>32) blk=128;
            break;
    case MC_SHA3 :
            blk=200-2*hlen;
            break;
    default: break;
    }
    return blk;
}


/* RFC 2104 */
void core::HMAC(int hash,int hlen,octet *TAG,int olen,octet *K,octet *M)
{
    int blk;
    char h[128],k0[200];   // assumes max block sizes
    octet K0 = {0, sizeof(k0), k0};
    octet H={0,sizeof(h),h};

    blk=blksize(hash,hlen);
    if (blk==0) return;

    if (K->len > blk) SPhash(hash,hlen,&K0,K);
    else              OCT_copy(&K0,K);

    OCT_jbyte(&K0,0,blk-K0.len); 
    OCT_xorbyte(&K0,0x36);

    GPhash(hash,hlen,&H,0,0,&K0,-1,M); 

    OCT_xorbyte(&K0,0x6a);   /* 0x6a = 0x36 ^ 0x5c */
    GPhash(hash,hlen,&H,0,0,&K0,-1,&H);

    OCT_empty(TAG);
    OCT_jbytes(TAG,H.val,olen);

    OCT_clear(&H);
    OCT_clear(&K0);
}

/* RFC 5869 */

void core::HKDF_Extract(int hash,int hlen,octet *PRK,octet *SALT,octet *IKM)
{
    char h[64];
    octet H={0,sizeof(h),h};
    if (SALT==NULL) {
        OCT_jbyte(&H,0,hlen);
        HMAC(hash,hlen,PRK,hlen,&H,IKM);
    } else {
        HMAC(hash,hlen,PRK,hlen,SALT,IKM);
    }
}

void core::HKDF_Expand(int hash,int hlen,octet *OKM,int olen,octet *PRK,octet *INFO)
{
    int i;
    char t[1024];  // >= info.length+hlen+1
    octet T={0,sizeof(t),t};
    int n=olen/hlen; 
    int flen=olen%hlen;
    OCT_empty(OKM);

    for (i=1;i<=n;i++)
    {
        OCT_joctet(&T,INFO);
        OCT_jbyte(&T,i,1);
        HMAC(hash,hlen,&T,hlen,PRK,&T);
        OCT_joctet(OKM,&T);
    }
    if (flen>0)
    {
        OCT_joctet(&T,INFO);
        OCT_jbyte(&T,n+1,1);
        HMAC(hash,hlen,&T,flen,PRK,&T);
        OCT_joctet(OKM,&T);
    }
}

/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */

void core::XOF_Expand(int hlen,octet *OKM,int olen,octet *DST,octet *M)
{
    int i;
    sha3 SHA3;
    SHA3_init(&SHA3,hlen);
    for (i=0;i<M->len;i++) SHA3_process(&SHA3,M->val[i]);
    SHA3_process(&SHA3,olen/256);
    SHA3_process(&SHA3,olen%256);

    for (i=0;i<DST->len;i++)
        SHA3_process(&SHA3,DST->val[i]);
    SHA3_process(&SHA3,DST->len);

    SHA3_shake(&SHA3,OKM->val,olen);
    OKM->len=olen;
}

void core::XMD_Expand(int hash, int hlen,octet *OKM,int olen,octet *DST,octet *M)
{
    int i,blk;
    int ell=CEIL(olen,hlen);
    char tmp[260]; 
    octet TMP={0,sizeof(tmp),tmp};
    char h0[64];
    octet H0 = {0, sizeof(h0), h0};
    char h1[64];
    octet H1 = {0, sizeof(h1), h1};

    blk=blksize(hash,hlen);
    OCT_jint(&TMP,olen,2);
    OCT_jint(&TMP,0,1);
    OCT_joctet(&TMP,DST);
    OCT_jint(&TMP,DST->len,1);

    GPhash(hash,hlen,&H0,0,blk,M,-1,&TMP);
    OCT_empty(&TMP);
    OCT_jint(&TMP,1,1);
    OCT_joctet(&TMP,DST);
    OCT_jint(&TMP,DST->len,1);

    GPhash(hash,hlen,&H1,0,0,&H0,-1,&TMP);
    OCT_empty(OKM);
    OCT_joctet(OKM,&H1);
    for (i=2;i<=ell;i++)
    {
        OCT_xor(&H1,&H0);
        OCT_empty(&TMP);
        OCT_jint(&TMP,i,1);
        OCT_joctet(&TMP,DST);
        OCT_jint(&TMP,DST->len,1);
        GPhash(hash,hlen,&H1,0,0,&H1,-1,&TMP);
        OCT_joctet(OKM,&H1);
    }
    OKM->len=olen; 
}

/* Key Derivation Function */

void core::KDF2(int hash, int hlen, octet *key, int olen, octet *z, octet *p)
{
    /* NOTE: the parameter olen is the length of the output k in bytes */
    char h[64];
    octet H = {0, sizeof(h), h};
    int counter, cthreshold;

    OCT_empty(key);

    cthreshold = ROUNDUP(olen, hlen);

    for (counter = 1; counter <= cthreshold; counter++)
    {
        GPhash(hash,hlen, &H, 0, 0, z, counter, p);
        if (key->len + hlen > olen)  OCT_jbytes(key, H.val, olen % hlen);
        else                     OCT_joctet(key, &H);
    }

}

/* Password based Key Derivation Function */
/* Input password p, salt s, and repeat count */
/* Output key of length olen */
void core::PBKDF2(int hash, int hlen, octet *key, int olen, octet *p, octet *s, int rep)
{
    int i, j, len, d = ROUNDUP(olen, hlen);
    char f[64], u[64];
    octet F = {0, sizeof(f), f};
    octet U = {0, sizeof(u), u};
    OCT_empty(key);

    for (i = 1; i <= d; i++)
    {
        len = s->len;
        OCT_jint(s, i, 4);

        HMAC(hash, hlen, &F, hlen, s, p);

        s->len = len;
        OCT_copy(&U, &F);
        for (j = 2; j <= rep; j++)
        {
            HMAC(hash, hlen, &U, hlen, &U, p);
            OCT_xor(&F, &U);
        }

        OCT_joctet(key, &F);
    }

    OCT_chop(key, NULL, olen);
}

/* RSA Auxiliary Functions */

#define MAX_RSA_BYTES 512 /**< Maximum of 4096 */

/* Mask Generation Function */

static void MGF1(int sha, octet *z, int olen, octet *mask)
{
    char h[64];
    octet H = {0, sizeof(h), h};
    int hlen = sha;
    int counter, cthreshold;

    OCT_empty(mask);

    cthreshold = ROUNDUP(olen, hlen);
    for (counter = 0; counter < cthreshold; counter++)
    {
        GPhash(MC_SHA2,sha,&H,0,0,z,counter,NULL);
        //hashit(sha, z, counter, &H);
        if (mask->len + hlen > olen) OCT_jbytes(mask, H.val, olen % hlen);
        else                     OCT_joctet(mask, &H);
    }
    OCT_clear(&H);
}

/* MGF1 plus masking */
static void MGF1XOR(int sha, octet *z, octet *w)
{
    char h[64];
    octet H = {0, sizeof(h), h};
    int i,len,wlen,hlen = sha;
    int counter, cthreshold;

    wlen=0;
    cthreshold = ROUNDUP(w->len, hlen);
    for (counter = 0; counter < cthreshold; counter++)
    {
        GPhash(MC_SHA2,sha,&H,0,0,z,counter,NULL);
        if (wlen+hlen <= w->len)
            len=hlen;
        else 
            len=w->len%hlen;
  
        for (i=0;i<len;i++)
            w->val[wlen+i]^=H.val[i];
        wlen+=len;
    }      
    OCT_clear(&H);
}


/* SHAXXX identifier strings */
const unsigned char SHA256ID[] = {0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20};
const unsigned char SHA384ID[] = {0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30};
const unsigned char SHA512ID[] = {0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40};

/* PKCS 1.5 padding of a message to be signed */

int core::PKCS15(int sha, octet *m, octet *w)
{
    int olen = w->max;
    int hlen = sha;
    int idlen = 19;
    char h[64];
    octet H = {0, sizeof(h), h};

    if (olen < idlen + hlen + 10) return 0;
    GPhash(MC_SHA2,sha,&H,0,0,m,-1,NULL);
    //hashit(sha, m, -1, &H);

    OCT_empty(w);
    OCT_jbyte(w, 0x00, 1);
    OCT_jbyte(w, 0x01, 1);
    OCT_jbyte(w, 0xff, olen - idlen - hlen - 3);
    OCT_jbyte(w, 0x00, 1);

    if (hlen == 32) OCT_jbytes(w, (char *)SHA256ID, idlen);
    if (hlen == 48) OCT_jbytes(w, (char *)SHA384ID, idlen);
    if (hlen == 64) OCT_jbytes(w, (char *)SHA512ID, idlen);

    OCT_joctet(w, &H);

    return 1;
}

/* PSS Encoding of message to be signed. Salt is hlen */

int core::PSS_ENCODE(int sha, octet *m, csprng *RNG, octet *w)
{ 
    int i,k;
    unsigned char mask;
    char h[64];
    octet H = {0, sizeof(h), h};
    char md[136];
    octet MD={0,sizeof(md),md};
    char  salt[64];
    octet SALT={0,sizeof(salt),salt};
    int hlen=sha;
    int emlen=w->max;
    int embits=8*emlen-1;

    OCT_rand(&SALT, RNG, hlen);
 //   for (i=0;i<hlen;i++)
 //       SALT.val[i]=i+1;
    

    mask=(0xff)>>(8*emlen-embits);

    GPhash(MC_SHA2,sha,&H,0,0,m,-1,NULL);
    if (emlen < hlen + hlen +  2) return 0; 

    OCT_jbyte(&MD,0,8);
    OCT_joctet(&MD,&H);
    OCT_joctet(&MD,&SALT);

    GPhash(MC_SHA2,sha,&H,0,0,&MD,-1,NULL);
    OCT_clear(w);
    OCT_jbyte(w,0,emlen-hlen-hlen-2);
    OCT_jbyte(w,0x01,1);
    OCT_joctet(w,&SALT);
    MGF1XOR(sha,&H,w);
    w->val[0]&=mask;

    OCT_joctet(w,&H);
    OCT_jbyte(w,0xbc,1);

    return 1;
}

int core::PSS_VERIFY(int sha, octet *m,octet *w)
{
    int i,k;
    unsigned char mask;
    char hmask[64];
    octet HMASK = {0, sizeof(hmask), hmask};
    char h[64];
    octet H = {0, sizeof(h), h};
    char db[MAX_RSA_BYTES];
    octet DB = {0, sizeof(db), db};
    char salt[64];
    octet SALT={0,sizeof(salt),salt};
    int hlen=sha;
    int emlen=w->len;
    int embits=8*emlen-1;

    mask=(0xff)>>(8*emlen-embits);

    GPhash(MC_SHA2,sha,&HMASK,0,0,m,-1,NULL);
    if (emlen < hlen + hlen +  2) return 0; 
    if (w->val[emlen-1]!=(char)0xbc) return 0;
    if ((w->val[0]&(~mask))!=0) return 0;

    OCT_jbytes(&DB,w->val,emlen-hlen-1);
    OCT_jbytes(&H,&w->val[emlen-hlen-1],hlen);
    
    MGF1XOR(sha,&H,&DB);
    DB.val[0]&=mask;

    k=0;
    for (i=0;i<emlen-hlen-hlen-2;i++)
        k|=DB.val[i];
    if (k!=0) return 0;
    if (DB.val[emlen-hlen-hlen-2]!=0x01) return 0;
    OCT_jbytes(&SALT,&DB.val[DB.len-hlen],hlen);
    
    OCT_clear(&DB);
    OCT_jbyte(&DB,0,8);
    OCT_joctet(&DB,&HMASK);
    OCT_joctet(&DB,&SALT);
    GPhash(MC_SHA2,sha,&HMASK,0,0,&DB,-1,NULL);

    if (!OCT_ncomp(&H,&HMASK,H.len))
        return 0;

    return 1;
}

/* OAEP Message Encoding for Encryption */

int core::OAEP_ENCODE(int sha, octet *m, csprng *RNG, octet *p, octet *f)
{
    int slen, olen = f->max - 1;
    int mlen = m->len;
    int hlen, seedlen;
    char dbmask[MAX_RSA_BYTES], seed[64];
    octet DBMASK = {0, sizeof(dbmask), dbmask};
    octet SEED = {0, sizeof(seed), seed};

    hlen = seedlen = sha;
    if (mlen > olen - hlen - seedlen - 1) return 0;
    if (m == f) return 0; /* must be distinct octets */

    GPhash(MC_SHA2,sha,f,0,0,p,-1,NULL);
    //hashit(sha, p, -1, f);

    slen = olen - mlen - hlen - seedlen - 1;

    OCT_jbyte(f, 0, slen);
    OCT_jbyte(f, 0x1, 1);
    OCT_joctet(f, m);

    OCT_rand(&SEED, RNG, seedlen);

    MGF1(sha, &SEED, olen - seedlen, &DBMASK);

    OCT_xor(&DBMASK, f);
    MGF1(sha, &DBMASK, seedlen, f);

    OCT_xor(f, &SEED);

    OCT_joctet(f, &DBMASK);

    OCT_pad(f, f->max);
    OCT_clear(&SEED);
    OCT_clear(&DBMASK);

    return 1;
}

/* OAEP Message Decoding for Decryption */

int core::OAEP_DECODE(int sha, octet *p, octet *f)
{
    int comp, x, t;
    int i, k, olen = f->max - 1;
    int hlen, seedlen;
    char dbmask[MAX_RSA_BYTES], seed[64], chash[64];
    octet DBMASK = {0, sizeof(dbmask), dbmask};
    octet SEED = {0, sizeof(seed), seed};
    octet CHASH = {0, sizeof(chash), chash};

    seedlen = hlen = sha;
    if (olen < seedlen + hlen + 1) return 0;
    if (!OCT_pad(f, olen + 1)) return 0;

    GPhash(MC_SHA2,sha,&CHASH,0,0,p,-1,NULL);
    //hashit(sha, p, -1, &CHASH);

    x = f->val[0];
    for (i = seedlen; i < olen; i++)
        DBMASK.val[i - seedlen] = f->val[i + 1];
    DBMASK.len = olen - seedlen;

    MGF1(sha, &DBMASK, seedlen, &SEED);
    for (i = 0; i < seedlen; i++) SEED.val[i] ^= f->val[i + 1];
    MGF1(sha, &SEED, olen - seedlen, f);
    OCT_xor(&DBMASK, f);

    comp = OCT_ncomp(&CHASH, &DBMASK, hlen);

    OCT_shl(&DBMASK, hlen);

    OCT_clear(&SEED);
    OCT_clear(&CHASH);

    for (k = 0;; k++)
    {
        if (k >= DBMASK.len)
        {
            OCT_clear(&DBMASK);
            return 0;
        }
        if (DBMASK.val[k] != 0) break;
    }

    t = DBMASK.val[k];
    if (!comp || x != 0 || t != 0x01)
    {
        OCT_clear(&DBMASK);
        return 0;
    }

    OCT_shl(&DBMASK, k + 1);
    OCT_copy(f, &DBMASK);
    OCT_clear(&DBMASK);

    return 1;
}



/* g++ -O2 hmac.cpp oct.cpp hash.cpp rand.cpp -o hmac 

int main()
{
    char dst[100],msg[100],okm[100];
    octet DST={0,sizeof(dst),dst};
    octet MSG={0,sizeof(msg),msg};
    octet OKM={0,sizeof(okm),okm};

    OCT_jstring(&MSG,(char *)"abc");
    OCT_jstring(&DST,(char *)"P256_XMD:SHA-256_SSWU_RO_TESTGEN");
//    XMD_Expand(MC_SHA2,32,&OKM,48,&DST,&MSG);
    XOF_Expand(SHAKE128,&OKM,48,&DST,&MSG);

    printf("OKM= %d ",OKM.len); OCT_output(&OKM);

    char ikm[22],salt[13],prk[32],info[10],okm[50];  
    octet IKM = {0, sizeof(ikm), ikm};
    octet SALT={0,sizeof(salt),salt};
    octet PRK={0,sizeof(prk),prk};
    octet OKM={0,sizeof(okm),okm};
    octet INFO={0,sizeof(info),info};
    int i;
    for (i=0;i<22;i++) IKM.val[i]=0x0b;
    for (i=0;i<13;i++) SALT.val[i]=i;
    for (i=0;i<10;i++) INFO.val[i]=0xf0+i;
    
    IKM.len=22; SALT.len=13; INFO.len=10;

    printf("IKM= "); OCT_output(&IKM); 
    printf("SALT= "); OCT_output(&SALT); 

    HKDF_Extract(MC_SHA2,32,&PRK,&SALT,&IKM);

    //HMAC(&PRK,32,&SALT,&IKM,SHA2,32);

    printf("PRK= "); OCT_output(&PRK); 

    HKDF_Expand(MC_SHA2,32,&OKM,42,&PRK,&INFO);

    printf("OKM= %d ",OKM.len); OCT_output(&OKM);  
}

*/
