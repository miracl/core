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


        GPhashit: function(hash, sha, pad, A, n, B) {
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
            return this.GPhashit(hash,sha, 0, A, -1 ,null);
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
                B = this.GPhashit(hash, sha, 0, Z, counter, P);

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

                this.HMAC1(hash, sha, F, sha, Pass, S);

                for (j = 0; j < sha; j++) {
                    U[j] = F[j];
                }

                for (j = 2; j <= rep; j++) {
                    this.HMAC1(hash, sha, U, sha, Pass, U);
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

        HMAC1: function(hash, sha, tag, olen, K, M) {
            /* Input is from an octet m        *
             * olen is requested output length in bytes. k is the key  *
             * The output is the calculated tag */
            var B = [],
                b = 0,
                K0, i;

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

            B = this.GPhashit(hash, sha, 0, K0, -1, M);

            for (i = 0; i < b; i++) {
                K0[i] ^= 0x6a;
            }

            B = this.GPhashit(hash, sha, olen, K0, -1, B);

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
        }
    };

    return HMAC;
};


/*
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
