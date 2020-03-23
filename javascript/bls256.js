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

/* BLS API Functions */

var BLS256 = function(ctx) {
    "use strict";

    var BLS256 = {
        BLS_OK: 0,
        BLS_FAIL: -1,

        BFS: ctx.BIG.MODBYTES,
        BGS: ctx.BIG.MODBYTES,
        G2_TAB: [],

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

        asciitobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i++) {
                b.push(s.charCodeAt(i));
            }

            return b;
        },

        stringtobytes: function(s) {
            var b = [],
                i;

            for (i = 0; i < s.length; i+=2) {
                b.push(parseInt(s.substr(i,2),16));
            }

            return b;
        },

        init: function() {
            var G = ctx.ECP8.generator();
			if (G.is_infinity()) return this.BLS_FAIL;
            this.G2_TAB = ctx.PAIR256.precomp(G);
			return this.BLS_OK;
        },

        ceil: function(a,b) {
            return Math.floor(((a)-1)/(b)+1);
        },

/* output u \in F_p */
        hash_to_field: function(hash,hlen,DST,M,ctr) {
            var q = new ctx.BIG(0);
            q.rcopy(ctx.ROM_FIELD.Modulus);
            var L = this.ceil(q.nbits()+ctx.ECP.AESKEY*8,8);
            var u=[];
            var fd=[];
            var OKM=ctx.HMAC.XMD_Expand(hash,hlen,L*ctr,DST,M);

            for (var i=0;i<ctr;i++)
            {
                for (var j=0;j<L;j++)
                    fd[j]=OKM[i*L+j];
                u[i]=new ctx.FP(ctx.DBIG.fromBytes(fd).mod(q));
            }
            return u;
        },

/* output u \in F_p 
        hash_to_base: function(hash,hlen,DST,M,ctr) {
            var q = new ctx.BIG(0);
            q.rcopy(ctx.ROM_FIELD.Modulus);
            var L = this.ceil(q.nbits()+ctx.ECP.AESKEY*8,8);

            var tag= "H2C";
            var INFO=this.asciitobytes(tag);
            INFO[INFO.length]=ctr;

            var PRK=ctx.HMAC.HKDF_Extract(hash,hlen,DST,M);
            var OKM=ctx.HMAC.HKDF_Expand(hash,hlen,L,PRK,INFO);

            var dx=ctx.DBIG.fromBytes(OKM);
            var u=dx.mod(q);
            return u;
        }, */

        /* hash a message to an ECP point, using SHA3 */

        bls_hash_to_point: function(M) {
            var dst= "BLS_SIG_ZZZG1_XMD:SHA512-SSWU-RO-_NUL_";
            var u=this.hash_to_field(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,this.asciitobytes(dst),M,2);

            var P=ctx.ECP.map2point(u[0]);
            var P1=ctx.ECP.map2point(u[1]);
            P.add(P1);
            P.cfp();
            P.affine();
            return P;
        },

        /* generate key pair, private key S, public key W */

        KeyPairGenerate: function(IKM, S, W) {
            var r = new ctx.BIG(0);
            r.rcopy(ctx.ROM_CURVE.CURVE_Order); 
            var L = this.ceil(3*this.ceil(r.nbits(),8),2);
            var G = ctx.ECP8.generator();
            var salt="BLS-SIG-KEYGEN-SALT-";
            var info="";
            var PRK=ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,this.asciitobytes(salt),IKM);
            var OKM=ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,L,PRK,this.asciitobytes(info));

            var dx=ctx.DBIG.fromBytes(OKM);
            var s=dx.mod(r);

            s.toBytes(S);
            G = ctx.PAIR256.G2mul(G, s);
            G.toBytes(W,true);
            return this.BLS_OK;
        },

        /* Sign message m using private key S to produce signature SIG */

        core_sign: function(SIG, M, S) {
            var D = this.bls_hash_to_point(M);
            var s = ctx.BIG.fromBytes(S);
            D = ctx.PAIR256.G1mul(D, s);
            D.toBytes(SIG, true);
            return this.BLS_OK;
        },

        /* Verify signature given message m, the signature SIG, and the public key W */

        core_verify: function(SIG, M, W) {
            var HM = this.bls_hash_to_point(M);

            var D = ctx.ECP.fromBytes(SIG);
            if (!ctx.PAIR256.G1member(D)) return this.BLS_FAIL;
            D.neg();

            var PK = ctx.ECP8.fromBytes(W);
            
            //if (!ctx.PAIR256.G2member(PK)) alert("Not a member");
    //alert("PK= "+PK.toString());

            // Use new multi-pairing mechanism 
            var r = ctx.PAIR256.initmp();
            //			ctx.PAIR256.another(r,G,D);
            ctx.PAIR256.another_pc(r, this.G2_TAB, D);
            ctx.PAIR256.another(r, PK, HM);
            var v = ctx.PAIR256.miller(r);

            //.. or alternatively
            //var G = ctx.ECP8.generator();
            //var v=ctx.PAIR256.ate2(G,D,PK,HM);

            v = ctx.PAIR256.fexp(v);
            if (v.isunity())
                return this.BLS_OK;
            return this.BLS_FAIL;
        }
    };

    return BLS256;
};