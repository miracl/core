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

/* Hybrid Public Key Encryption */

/* Following https://datatracker.ietf.org/doc/draft-irtf-cfrg-hpke/?include_text=1 */

var HPKE = function(ctx) {
    "use strict";

    var HPKE = {

        reverse: function(X) {
	        var i,ch,lx=X.length;
	        for (i=0;i<lx/2;i++)  {
		        ch =X[i];
		        X[i]=X[lx-i-1];
		        X[lx-i-1]=ch;
	        }
        },

        encap: function(config_id,RNG,SKE,pkE,pkR) {
            var skE = [];
            var Z = [];
	        var kem = config_id&7;
	        if (RNG == null) {
		        for (var i=0;i<ctx.ECDH.EGS;i++) 
			        skE[i]=SKE[i];
		        if (kem==2 || kem==4) {
			        this.reverse(skE);
			        if (kem==2) {
				        skE[ctx.ECDH.EGS-1]&=248;
				        skE[0]&=127;
				        skE[0]|=64;
			        } else {
				        skE[ctx.ECDH.EGS-1]&=252;
				        skE[0]|=128;
			        }
		        }
	        }
	        ctx.ECDH.KEY_PAIR_GENERATE(RNG, skE, pkE);
	        if (kem==2 || kem==4) 
		        this.reverse(pkR);
	        ctx.ECDH.ECPSVDP_DH(skE, pkR, Z);
	        if (kem==2 || kem==4) {
		        this.reverse(pkR);
		        this.reverse(pkE);
		        this.reverse(Z);
	        }
    	    return Z;
        },

        decap: function(config_id,pkE,SKR) {
            var skR = [];
            var Z = [];
	        var kem = config_id&7;
	        for (var i=0;i<ctx.ECDH.EGS;i++)
		        skR[i]=SKR[i];
	        if (kem==2 || kem==4) {
		        this.reverse(skR);
		        this.reverse(pkE);
		        if (kem==2) {
			        skR[ctx.ECDH.EGS-1]&=248;
			        skR[0]&=127;
			        skR[0]|=64;
		        } else {
			        skR[ctx.ECDH.EGS-1]&=252;
			        skR[0]|=128;
		        }
	        }
	        ctx.ECDH.ECPSVDP_DH(skR, pkE, Z);
	            if (kem==2 || kem==4) {
		        this.reverse(pkE);
		        this.reverse(Z);
	        }
	        return Z;
        },

        authEncap: function(config_id,RNG,SKE,pkE,pkR,SKI) {
            var skE = [];
            var Z = [];
            var skI = [];
            var NZ = [];
	
	        var kem = config_id&7;

	        if (RNG == null) {
		        for (var i=0;i<ctx.ECDH.EGS;i++) {
			        skE[i]=SKE[i];
			        skI[i]=SKI[i];
		        }	
		        if (kem==2 || kem==4) {
			        this.reverse(skE);
			        this.reverse(skI);
			        if (kem==2) {
				        skE[ctx.ECDH.EGS-1]&=248;
				        skE[0]&=127;
				        skE[0]|=64;
				        skI[ctx.ECDH.EGS-1]&=248;
				        skI[0]&=127;
				        skI[0]|=64;	
			        } else {
				        skE[ctx.ECDH.EGS-1]&=252;
				        skE[0]|=128;
				        skI[ctx.ECDH.EGS-1]&=252;
				        skI[0]|=128;
			        }
		        }
	        }
	        ctx.ECDH.KEY_PAIR_GENERATE(RNG, skE, pkE);
	        if (kem==2 || kem==4) 
		        this.reverse(pkR);
	
	        ctx.ECDH.ECPSVDP_DH(skE, pkR, Z);
	        ctx.ECDH.ECPSVDP_DH(skI, pkR, NZ);
	        if (kem==2 || kem==4) {
		        this.reverse(pkR);
		        this.reverse(pkE);
		        this.reverse(Z);
		        this.reverse(NZ);
	        }
	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = Z[i];
                ZZ[ctx.ECDH.EFS+i]= NZ[i];
	        }
	        return ZZ;
        },

        authDecap: function(config_id,pkE,SKR,pkI) {
            var skR = [];
            var Z = [];
            var NZ = [];
		    var kem = config_id&7;

	        for (var i=0;i<ctx.ECDH.EGS;i++) 
		        skR[i]=SKR[i];
	
	        if (kem==2 || kem==4) {
		        this.reverse(skR);
		        this.reverse(pkE);
		        this.reverse(pkI);
		        if (kem==2) {
			        skR[ctx.ECDH.EGS-1]&=248;
			        skR[0]&=127;
			        skR[0]|=64;
		        } else {
		    	    skR[ctx.ECDH.EGS-1]&=252;
			        skR[0]|=128;
		        }  
	        }
	        ctx.ECDH.ECPSVDP_DH(skR, pkE, Z);
	        ctx.ECDH.ECPSVDP_DH(skR, pkI, NZ);
	        if (kem==2 || kem==4) {
		        this.reverse(pkI);
		        this.reverse(pkE);
		        this.reverse(Z);
		        this.reverse(NZ);
	        }
	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = Z[i];
                ZZ[ctx.ECDH.EFS+i]= NZ[i];
	        }
	        return ZZ;
        },

        keySchedule: function(config_id,key,nonce,mode,pkR,Z,pkE,info,psk,pskID,pkI) {
            var ctxlen=7+3*pkR.length+2*ctx.ECP.HASH_TYPE;
	        var context = [];
	        var kem_id=config_id&7;
	        var kdf_id=(config_id>>3)&3;
	        var aead_id=(config_id>>5)&3;
            var k=0;

	        var ar = ctx.HMAC.inttobytes(mode, 1);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];
	
	        ar=ctx.HMAC.inttobytes(kem_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        ar=ctx.HMAC.inttobytes(kdf_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        ar=ctx.HMAC.inttobytes(aead_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        for (var i=0;i<pkE.length;i++) 
		        context[k++] = pkE[i];
	
	        for (var i=0;i<pkR.length;i++) 
		        context[k++] = pkR[i];

	        if (pkI==null) {
		        for (var i=0;i<pkR.length;i++)
			        context[k++] = 0;
	        } else {
		        for (var i=0;i<pkI.length;i++)
			        context[k++] = pkI[i];
	        }
	        var H=ctx.HMAC.SPhashit(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,pskID);
	        for (var i=0;i<ctx.ECP.HASH_TYPE;i++) 
		        context[k++] = H[i];
	
	        H=ctx.HMAC.SPhashit(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,info);
	        for (var i=0;i<ctx.ECP.HASH_TYPE;i++) 
		        context[k++] = H[i];

	        var secret=ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,psk,Z);
	
	        var pp = "hpke key";
	        ar = ctx.ECDH.asciitobytes(pp); k=0;
            var full_context = []; 
	        for (var i=0;i<ar.length;i++)
		        full_context[k++] = ar[i];
	        for (var i=0;i<ctxlen;i++)
		        full_context[k++] = context[i];
	
	        var ex=ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,ctx.ECP.AESKEY,secret,full_context);
            for (var i=0;i<ex.length;i++) key[i]=ex[i];

	        pp = "hpke nonce";
	        ar = ctx.ECDH.asciitobytes(pp); k=0;
            full_context = []; 
	        for (var i=0;i<ar.length;i++)
		        full_context[k++] = ar[i];
	        for (var i=0;i<ctxlen;i++)
		        full_context[k++] = context[i];
	
	        ex=ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,12,secret,full_context);
            for (var i=0;i<ex.length;i++) nonce[i]=ex[i];
        }

    };
    return HPKE;
};
