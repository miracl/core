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

var HMAC = function(ctx) {
    "use strict";

    var HMAC = {

        MC_SHA2: 2,
        MC_SHA3: 3,

        /* Convert Integer to n-byte array */
        inttobytes: function(n, len) {
            var b = [],
                i;

            for (i = 0; i < len; i++) {
                b[i] = 0;
            }

            i = len;
            while (n > 0 && i > 0) {
                i--;
                b[i] = (n & 0xff);
                n = Math.floor(n / 256);
            }

            return b;
        },


        GPhashit: function(hash, sha, pad, zpad, A, n, B) {
            var R = [],
                H, W, i, len;

            if (hash == this.MC_SHA2)
            {
                if (sha == 32) {
                    H = new ctx.HASH256();
                } else if (sha == 48) {
                    H = new ctx.HASH384();
                } else if (sha == 64) {
                    H = new ctx.HASH512();
                }
            }
            if (hash == this.MC_SHA3)
            {
                H = new ctx.SHA3(sha);
            }

            for (i=0;i<zpad;i++)
                H.process(0);

            if (A != null) {
                H.process_array(A);
            }

            if (n >= 0) {
                H.process_num(n);
            }
            if (B != null) {
                H.process_array(B);
            }
            R = H.hash();

            if (R.length == 0) {
                return null;
            }

            if (pad == 0) {
                return R;
            }

            W = [];

            len = pad;

            if (sha >= len) {
                for (i = 0; i < len; i++) {
                    W[i] = R[i];
                }
            } else {
                for (i = 0; i < sha; i++) {
                    W[i + len - sha] = R[i];
                }

                for (i = 0; i < len - sha; i++) {
                    W[i] = 0;
                }
            }

            return W;
        },

        SPhashit: function(hash, sha, A) {
            return this.GPhashit(hash,sha, 0, 0, A, -1 ,null);
        },

        KDF2: function(hash,sha, Z, P, olen) {
            /* NOTE: the parameter olen is the length of the output k in bytes */
            var hlen = sha,
                K = [],
                B = [],
                k = 0,
                counter, cthreshold, i;

            for (i = 0; i < K.length; i++) {
                K[i] = 0; // redundant?
            }

            cthreshold = Math.floor(olen / hlen);
            if (olen % hlen !== 0) {
                cthreshold++;
            }

            for (counter = 1; counter <= cthreshold; counter++) {
                B = this.GPhashit(hash, sha, 0, 0, Z, counter, P);

                if (k + hlen > olen) {
                    for (i = 0; i < olen % hlen; i++) {
                        K[k++] = B[i];
                    }
                } else {
                    for (i = 0; i < hlen; i++) {
                        K[k++] = B[i];
                    }
                }
            }

            return K;
        },

        /* Password based Key Derivation Function */
        /* Input password p, salt s, and repeat count */
        /* Output key of length olen */

        PBKDF2: function(hash, sha, Pass, Salt, rep, olen) {
            var F = new Array(sha),
                U = [],
                S = [],
                K = [],
                opt = 0,
                i, j, k, d, N, key;

            d = Math.floor(olen / sha);

            if (olen % sha !== 0) {
                d++;
            }

            opt = 0;

            for (i = 1; i <= d; i++) {
                for (j = 0; j < Salt.length; j++) {
                    S[j] = Salt[j];
                }

                N = this.inttobytes(i, 4);

                for (j = 0; j < 4; j++) {
                    S[Salt.length + j] = N[j];
                }

                this.HMAC1(hash, sha, F, sha, S, Pass);

                for (j = 0; j < sha; j++) {
                    U[j] = F[j];
                }

                for (j = 2; j <= rep; j++) {
                    this.HMAC1(hash, sha, U, sha, U, Pass);
                    for (k = 0; k < sha; k++) {
                        F[k] ^= U[k];
                    }
                }

                for (j = 0; j < sha; j++) {
                    K[opt++] = F[j];
                }
            }

            key = [];
            for (i = 0; i < olen; i++) {
                key[i] = K[i];
            }

            return key;
        },

        blksize: function(hash,sha) {
            var b=0;
            if (hash==this.MC_SHA2)
            {
                b=64;
                if (sha>32)
                    b=128;
            }
            if (hash == this.MC_SHA3)
            {
                b=200-2*sha;
            }
            return b;
        },

        HMAC1: function(hash, sha, tag, olen, K, M) {
            /* Input is from an octet m        *
             * olen is requested output length in bytes. k is the key  *
             * The output is the calculated tag */
            var B = [],
                b = 0,
                K0, i;

            b=this.blksize(hash,sha);
            if (b==0) return 0;

            K0 = new Array(b);

            for (i = 0; i < b; i++) {
                K0[i] = 0;
            }

            if (K.length > b) {
                B = this.SPhashit(hash, sha, K);
                for (i = 0; i < sha; i++) {
                    K0[i] = B[i];
                }
            } else {
                for (i = 0; i < K.length; i++) {
                    K0[i] = K[i];
                }
            }

            for (i = 0; i < b; i++) {
                K0[i] ^= 0x36;
            }

            B = this.GPhashit(hash, sha, 0, 0, K0, -1, M);

            for (i = 0; i < b; i++) {
                K0[i] ^= 0x6a;
            }

            B = this.GPhashit(hash, sha, olen, 0, K0, -1, B);

            for (i = 0; i < olen; i++) {
                tag[i] = B[i];
            }

            return 1;
        },

        HKDF_Extract: function(hash, hlen,SALT,IKM) {
            var PRK=[];
            if (SALT==null)
            {
                var H=[];
                for (var i=0;i<hlen;i++) H[i]=0;
                this.HMAC1(hash,hlen,PRK,hlen,H,IKM);
            } else {
                this.HMAC1(hash,hlen,PRK,hlen,SALT,IKM);
            }
            return PRK;
        },

        HKDF_Expand: function(hash,hlen,olen,PRK,INFO) {
            var i,j,k,m,n=Math.floor(olen/hlen);
            var flen=olen%hlen;

            var OKM = [];
            var T = [];
            var K = [];

            k=m=0;
            for (i=1;i<=n;i++)
            {
                for (j=0;j<INFO.length;j++)
                    T[k++]=INFO[j];
                T[k++]=i;
                this.HMAC1(hash,hlen,K,hlen,PRK,T);
                k=0;
                T=[];
                for (j=0;j<hlen;j++)
                {
                    OKM[m++]=K[j];
                    T[k++]=K[j];
                }
            }
            if (flen>0)
            {
                for (j = 0; j < INFO.length; j++) 
                    T[k++] = INFO[j];
                T[k++] = (n+1);
                this.HMAC1(hash,hlen,K,flen,PRK,T);
                for (j = 0; j < flen; j++) 
                    OKM[m++] = K[j];
            }
            return OKM;
        },

        ceil: function(a,b) {
            return Math.floor(((a)-1)/(b)+1);
        },

        XOF_Expand: function(hlen,olen,DST,MSG) {
            var OKM = [];
            var H = new ctx.SHA3(hlen);
            for (var i=0;i<MSG.length;i++ )
                H.process(MSG[i]);
            H.process(((olen >> 8) & 0xff));
            H.process((olen & 0xff));

            for (var i=0;i<DST.length;i++ )
                H.process(DST[i]);
            H.process(DST.length);

            H.shake(OKM,olen);
            return OKM;
        },

        XMD_Expand_Short_DST(hash,hlen,olen,DST,MSG) {
            var OKM = [];
            var H1 = [];
            var TMP = [];
            var TMP2 = [];

            var ell=this.ceil(olen,hlen);
            var blk=this.blksize(hash,hlen);
            TMP[0]=(olen >> 8) & 0xff;
            TMP[1]=olen & 0xff;
            TMP[2]=0;
            for (var j=0;j<DST.length;j++)
                TMP[3+j]=DST[j];
            TMP[3+DST.length]=DST.length;

            var H0=this.GPhashit(hash, hlen, 0, blk, MSG, -1, TMP);

            var k=0;
            for (var j=0;j<hlen;j++)
               H1[j]=0;
        
            for (var i=1;i<=ell;i++)
            {
                for (var j=0;j<hlen;j++)
                    H1[j]^=H0[j];
                TMP2[0]=i;
                for (var j=0;j<DST.length;j++)
                    TMP2[1+j]=DST[j];
                TMP2[1+DST.length]=DST.length;
                H1=this.GPhashit(hash, hlen, 0, 0, H1, -1, TMP2);
                for (var j=0;j<hlen && k<olen;j++)
                    OKM[k++]=H1[j];
            }
        
            return OKM;
        },

        asciitobytes(s) {
            var b = [],i;
            for (i = 0; i < s.length; i++) {
                b.push(s.charCodeAt(i));
            }
            return b;
        },

        XMD_Expand(hash,hlen,olen,DST,MSG) {
            var R;
            var OS=this.asciitobytes("H2C-OVERSIZE-DST-");
            if (DST.length>=256)
            {
                var W=this.GPhashit(hash,hlen,0,0,OS,-1,DST);
                R=this.XMD_Expand_Short_DST(hash,hlen,olen,W,MSG);
            } else {
                R=this.XMD_Expand_Short_DST(hash,hlen,olen,DST,MSG);
            }
            return R;
        },

        SHA256: 32,
        SHA384: 48,
        SHA512: 64,

        /* SHAXXX identifier strings */
        SHA256ID: [0x30, 0x31, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x05, 0x00, 0x04, 0x20],
        SHA384ID: [0x30, 0x41, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x05, 0x00, 0x04, 0x30],
        SHA512ID: [0x30, 0x51, 0x30, 0x0d, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x05, 0x00, 0x04, 0x40],

        /* SHAXXX identifier strings */
        SHA256IDb: [0x30, 0x2f, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x01, 0x04, 0x20],
        SHA384IDb: [0x30, 0x3f, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x02, 0x04, 0x30],
        SHA512IDb: [0x30, 0x4f, 0x30, 0x0b, 0x06, 0x09, 0x60, 0x86, 0x48, 0x01, 0x65, 0x03, 0x04, 0x02, 0x03, 0x04, 0x40],


        /* Mask Generation Function */
        MGF1: function(sha, Z, olen, K) {
            var hlen = sha,
                B = [],
                k = 0,
                counter, cthreshold, i;

            for (i = 0; i < K.length; i++) {
                K[i] = 0;
            }

            cthreshold = Math.floor(olen / hlen);
            if (olen % hlen !== 0) {
                cthreshold++;
            }

            for (counter = 0; counter < cthreshold; counter++) {
                B = this.GPhashit(this.MC_SHA2, sha,0,0,Z,counter,null);
                //B = this.hashit(sha, Z, counter);

                if (k + hlen > olen) {
                    for (i = 0; i < olen % hlen; i++) {
                        K[k++] = B[i];
                    }
                } else {
                    for (i = 0; i < hlen; i++) {
                        K[k++] = B[i];
                    }
                }
            }
        },

        MGF1XOR: function(sha, Z, K) {
            var hlen = sha,
                B = [],
                k = 0,
                counter, cthreshold, i;
            var olen=K.length;

            cthreshold = Math.floor(olen / hlen);
            if (olen % hlen !== 0) {
                cthreshold++;
            }

            for (counter = 0; counter < cthreshold; counter++) {
                B = this.GPhashit(this.MC_SHA2, sha,0,0,Z,counter,null);
                //B = this.hashit(sha, Z, counter);

                if (k + hlen > olen) {
                    for (i = 0; i < olen % hlen; i++) {
                        K[k++]^=B[i];
                    }
                } else {
                    for (i = 0; i < hlen; i++) {
                        K[k++]^=B[i];
                    }
                }
            }
        },

        PKCS15: function(sha, m, w, RFS) {
            var olen = RFS,
                hlen = sha,
                idlen = 19,
                H, i, j;

            if (olen < idlen + hlen + 10) {
                return false;
            }
            H = this.SPhashit(this.MC_SHA2, sha, m);
            //H = this.hashit(sha, m, -1);

            for (i = 0; i < w.length; i++) {
                w[i] = 0;
            }

            i = 0;
            w[i++] = 0;
            w[i++] = 1;
            for (j = 0; j < olen - idlen - hlen - 3; j++) {
                w[i++] = 0xFF;
            }
            w[i++] = 0;

            if (hlen == this.SHA256) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA256ID[j];
                }
            } else if (hlen == this.SHA384) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA384ID[j];
                }
            } else if (hlen == this.SHA512) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA512ID[j];
                }
            }

            for (j = 0; j < hlen; j++) {
                w[i++] = H[j];
            }

            return true;
        },

// Alternate PKCS1.5
        PKCS15b: function(sha, m, w, RFS) {
            var olen = RFS,
                hlen = sha,
                idlen = 17,
                H, i, j;

            if (olen < idlen + hlen + 10) {
                return false;
            }
            H = this.SPhashit(this.MC_SHA2, sha, m);
            //H = this.hashit(sha, m, -1);

            for (i = 0; i < w.length; i++) {
                w[i] = 0;
            }

            i = 0;
            w[i++] = 0;
            w[i++] = 1;
            for (j = 0; j < olen - idlen - hlen - 3; j++) {
                w[i++] = 0xFF;
            }
            w[i++] = 0;

            if (hlen == this.SHA256) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA256IDb[j];
                }
            } else if (hlen == this.SHA384) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA384IDb[j];
                }
            } else if (hlen == this.SHA512) {
                for (j = 0; j < idlen; j++) {
                    w[i++] = this.SHA512IDb[j];
                }
            }

            for (j = 0; j < hlen; j++) {
                w[i++] = H[j];
            }

            return true;
        },

        PSS_ENCODE: function(sha,m,rng,RFS) {
            var emlen=RFS;
            var embits=8*emlen-1;
            var hlen=sha;
            var SALT=[];
            for (i = 0; i < hlen; i++) {
                SALT[i] = rng.getByte();
            }
            var mask=(0xff)>>(8*emlen-embits);

            var H = this.SPhashit(this.MC_SHA2, sha, m);

            if (emlen < hlen+hlen+2) return null;

            var MD=[];

            for (var i=0;i<8;i++)
                MD[i]=0;
            for (var i=0;i<hlen;i++)
                MD[8+i]=H[i];
            for (var i=0;i<hlen;i++)
                MD[8+hlen+i]=SALT[i];

            H=this.SPhashit(this.MC_SHA2, sha, MD);

            var f=[];
            for (var i=0;i<emlen-hlen-hlen-2;i++)
                f[i]=0;
            f[emlen-hlen-hlen-2]=0x1;
            for (var i=0;i<hlen;i++)
                f[emlen+i-hlen-hlen-1]=SALT[i];
            this.MGF1XOR(sha,H,f);
            f[0]&=mask;
            for (var i=0;i<hlen;i++)
                f[emlen+i-hlen-1]=H[i];
            f[emlen-1]=0xbc;
            return f;
        },

        PSS_VERIFY: function(sha,m,f) {
            var emlen=f.length;
            var embits=8*emlen-1;
            var hlen=sha;
            var SALT=[];
            var mask=(0xff)>>(8*emlen-embits);

            var HMASK = this.SPhashit(this.MC_SHA2, sha, m);
            if (emlen < hlen+hlen+2) return false;
            if (f[emlen-1]!=0xbc) return false;
            if ((f[0]&(~mask))!=0) return false;

            var DB=[];
            for (var i=0;i<emlen-hlen-1;i++)
                DB[i]=f[i];
            var H=[];
            for (var i=0;i<hlen;i++)
                H[i]=f[emlen+i-hlen-1];
            this.MGF1XOR(sha,H,DB);
            DB[0]&=mask;
            var k=0;
            for (var i=0;i<emlen-hlen-hlen-2;i++)
                k|=DB[i];
            if (k!=0) return false;
            if (DB[emlen-hlen-hlen-2]!=0x01) return false;

            for (var i=0;i<hlen;i++)
                SALT[i]=DB[emlen+i-hlen-hlen-1];

            var MD=[];
            for (var i=0;i<8;i++)
                MD[i]=0;
            for (var i=0;i<hlen;i++)
                MD[8+i]=HMASK[i];
            for (var i=0;i<hlen;i++)
                MD[8+hlen+i]=SALT[i];

            HMASK=this.SPhashit(this.MC_SHA2, sha, MD);

            k=0;
            for (var i=0;i<hlen;i++)
                k|=(H[i]-HMASK[i]);
            if (k!=0) return false;
                return true; 
        },
        /* OAEP Message Encoding for Encryption */
        OAEP_ENCODE: function(sha, m, rng, p, RFS) {
            var olen = RFS - 1,
                mlen = m.length,
                SEED = [],
                DBMASK = [],
                f = [],
                hlen,
                seedlen,
                slen,
                i, d, h;

            seedlen = hlen = sha;

            if (mlen > olen - hlen - seedlen - 1) {
                return null;
            }

            h = this.SPhashit(this.MC_SHA2, sha, p);
            //h = this.hashit(sha, p, -1);
            for (i = 0; i < hlen; i++) {
                f[i] = h[i];
            }

            slen = olen - mlen - hlen - seedlen - 1;

            for (i = 0; i < slen; i++) {
                f[hlen + i] = 0;
            }
            f[hlen + slen] = 1;
            for (i = 0; i < mlen; i++) {
                f[hlen + slen + 1 + i] = m[i];
            }

            for (i = 0; i < seedlen; i++) {
                SEED[i] = rng.getByte();
            }
            this.MGF1(sha, SEED, olen - seedlen, DBMASK);

            for (i = 0; i < olen - seedlen; i++) {
                DBMASK[i] ^= f[i];
            }
            this.MGF1(sha, DBMASK, seedlen, f);

            for (i = 0; i < seedlen; i++) {
                f[i] ^= SEED[i];
            }

            for (i = 0; i < olen - seedlen; i++) {
                f[i + seedlen] = DBMASK[i];
            }

            /* pad to length RFS */
            d = 1;
            for (i = RFS - 1; i >= d; i--) {
                f[i] = f[i - d];
            }
            for (i = d - 1; i >= 0; i--) {
                f[i] = 0;
            }

            return f;
        },

        /* OAEP Message Decoding for Decryption */
        OAEP_DECODE: function(sha, p, f, RFS) {

            var olen = RFS - 1,
                SEED = [],
                CHASH = [],
                DBMASK = [],
                comp,
                hlen,
                seedlen,
                x, t, d, i, k, h, r, m;

            seedlen = hlen = sha;
            if (olen < seedlen + hlen + 1) {
                return null;
            }

            for (i = 0; i < olen - seedlen; i++) {
                DBMASK[i] = 0;
            }

            if (f.length < RFS) {
                d = RFS - f.length;
                for (i = RFS - 1; i >= d; i--) {
                    f[i] = f[i - d];
                }
                for (i = d - 1; i >= 0; i--) {
                    f[i] = 0;
                }
            }
            h = this.SPhashit(this.MC_SHA2, sha, p);
            //h = this.hashit(sha, p, -1);
            for (i = 0; i < hlen; i++) {
                CHASH[i] = h[i];
            }

            x = f[0];

            for (i = seedlen; i < olen; i++) {
                DBMASK[i - seedlen] = f[i + 1];
            }

            this.MGF1(sha, DBMASK, seedlen, SEED);
            for (i = 0; i < seedlen; i++) {
                SEED[i] ^= f[i + 1];
            }
            this.MGF1(sha, SEED, olen - seedlen, f);
            for (i = 0; i < olen - seedlen; i++) {
                DBMASK[i] ^= f[i];
            }

            comp = 0;
            for (i = 0; i < hlen; i++) {
                comp |= (CHASH[i] ^ DBMASK[i]);
            }

            m=olen - seedlen - hlen;
            for (i = 0; i < m; i++) {
                DBMASK[i] = DBMASK[i + hlen];
            }

            for (i = 0; i < hlen; i++) {
                SEED[i] = CHASH[i] = 0;
            }

            t=k=0;
            for (i=0;i<m;i++) {
                if (t==0 && DBMASK[i]!=0)
                {
                    k=i;
                    t=DBMASK[i];
                }
            }

            for (k = 0;; k++) {
                if (k >= m) {
                    return null;
                }

                if (DBMASK[k] !== 0) {
                    break;
                }
            }
            t = DBMASK[k];

            if (comp != 0 || x !== 0 || t != 0x01) {
                for (i = 0; i < olen - seedlen; i++) {
                    DBMASK[i] = 0;
                }
                return null;
            }
            r = [];

            for (i = 0; i < m - k - 1; i++) {
                r[i] = DBMASK[i + k + 1];
            }

            for (i = 0; i < olen - seedlen; i++) {
                DBMASK[i] = 0;
            }

            return r;
        }
    };

    return HMAC;
};


/*
	var MSG=ctx.ECDH.asciitobytes("abc");
	var DST=ctx.ECDH.asciitobytes("P256_XMD:SHA-256_SSWU_RO_TESTGEN");

	var OKM=ctx.HMAC.XOF_Expand(ctx.SHA3.SHAKE128,48,DST,MSG);
	mywindow.document.write("OKM : 0x"+ctx.ECDH.bytestostring(OKM) + "<br>");

	OKM=ctx.HMAC.XMD_Expand(ctx.HMAC.MC_SHA2,32,48,DST,MSG);
	mywindow.document.write("OKM : 0x"+ctx.ECDH.bytestostring(OKM) + "<br>");

        ikm=[];
        salt=[];
        info=[];

        for (i=0;i<22;i++) ikm[i]=0x0b;
        for (i=0;i<13;i++) salt[i]=i;
        for (i=0;i<10;i++) info[i]=(0xf0+i);

        var prk=ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,32,salt,ikm);
        window.document.write("PRK= "+ctx.NHS.bytestostring(prk)+ "<br>");

        var okm=ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,32,42,prk,info);
        window.document.write("PRK= "+ctx.NHS.bytestostring(okm)+ "<br>");
*/

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        HMAC: HMAC
    };
}

