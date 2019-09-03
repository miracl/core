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


        /* hash a message to an ECP point, using SHA3 */

        bls_hashit: function(m) {
            var sh = new ctx.SHA3(ctx.SHA3.SHAKE256);
            var hm = [];
            var t = this.asciitobytes(m);
            for (var i = 0; i < t.length; i++)
                sh.process(t[i]);
            sh.shake(hm, this.BFS);
            var P = ctx.ECP.mapit(hm);
            return P;
        },

        /* generate key pair, private key S, public key W */

        KeyPairGenerate: function(rng, S, W) {
            var G = ctx.ECP8.generator();
			if (G.is_infinity()) return this.BLS_FAIL;
            var q = new ctx.BIG(0);
            q.rcopy(ctx.ROM_CURVE.CURVE_Order);
            var s = ctx.BIG.randtrunc(q, 16 * ctx.ECP.AESKEY, rng);
            s.toBytes(S);
            G = ctx.PAIR256.G2mul(G, s);
            G.toBytes(W, true); // To use point compression on public keys, change to true 
            return this.BLS_OK;

        },

        /* Sign message m using private key S to produce signature SIG */

        sign: function(SIG, m, S) {
            var D = this.bls_hashit(m);
            var s = ctx.BIG.fromBytes(S);
            D = ctx.PAIR256.G1mul(D, s);
            D.toBytes(SIG, true);
            return this.BLS_OK;
        },

        /* Verify signature given message m, the signature SIG, and the public key W */

        verify: function(SIG, m, W) {
            var HM = this.bls_hashit(m);

            var D = ctx.ECP.fromBytes(SIG);
            if (!ctx.PAIR256.G1member(D)) return this.BLS_FAIL;
            D.neg();

            var PK = ctx.ECP8.fromBytes(W);

            // Use new multi-pairing mechanism 
            var r = ctx.PAIR256.initmp();
            //			ctx.PAIR256.another(r,G,D);
            ctx.PAIR256.another_pc(r, this.G2_TAB, D);
            ctx.PAIR256.another(r, PK, HM);
            var v = ctx.PAIR256.miller(r);

            //.. or alternatively
            //			var v=ctx.PAIR256.ate2(G,D,PK,HM);

            v = ctx.PAIR256.fexp(v);
            if (v.isunity())
                return this.BLS_OK;
            return this.BLS_FAIL;
        }
    };

    return BLS256;
};