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

        LabeledExtract: function(SALT,SUITE_ID,label,IKM) {
            var rfc="HPKE-v1";
            var RFC=ctx.ECDH.asciitobytes(rfc);
            var LABEL=ctx.ECDH.asciitobytes(label);
            var LIKM = [];
            var k=0;
            for (var i=0;i<RFC.length;i++)
                LIKM[k++]=RFC[i];
            for (var i=0;i<SUITE_ID.length;i++)
                LIKM[k++]=SUITE_ID[i];
            for (var i=0;i<LABEL.length;i++)
                LIKM[k++]=LABEL[i];
            if (IKM!=null) {
                for (var i=0;i<IKM.length;i++)
                    LIKM[k++]=IKM[i];
            }
            return ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,SALT,LIKM);
        },

        LabeledExpand: function(PRK,SUITE_ID,label,INFO,L) {
            var AR = ctx.HMAC.inttobytes(L, 2);
            var rfc="HPKE-v1";
            var RFC=ctx.ECDH.asciitobytes(rfc);
            var LABEL=ctx.ECDH.asciitobytes(label);
            var LINFO = [];
            LINFO[0]=AR[0];
            LINFO[1]=AR[1];
            var k=2;
            for (var i=0;i<RFC.length;i++)
                LINFO[k++]=RFC[i];
            for (var i=0;i<SUITE_ID.length;i++)
                LINFO[k++]=SUITE_ID[i];
            for (var i=0;i<LABEL.length;i++)
                LINFO[k++]=LABEL[i];
            if (INFO!=null) {
                for (var i=0;i<INFO.length;i++)
                    LINFO[k++]=INFO[i];
            }
            return ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,L,PRK,LINFO);
        },

        ExtractAndExpand: function(config_id,DH,CONTEXT) {
	        var kem = config_id&255;
            var txt="KEM";
            var KEM_ID = ctx.HMAC.inttobytes(kem, 2);
            var KEM =ctx.ECDH.asciitobytes(txt);
            var SUITE_ID = [];
            var k=KEM.length;
            for (var i=0;i<k;i++)
                SUITE_ID[i]=KEM[i];
            SUITE_ID[k]=KEM_ID[0];
            SUITE_ID[k+1]=KEM_ID[1];
            var PRK=this.LabeledExtract(null,SUITE_ID,"eae_prk",DH);
            return this.LabeledExpand(PRK,SUITE_ID,"shared_secret",CONTEXT,ctx.ECP.HASH_TYPE);
        },

        DeriveKeyPair: function(config_id,SK,PK,SEED) {
            var counter=0;
            var INFO=[];
	        var kem = config_id&255;
            var txt="KEM";
            var KEM_ID = ctx.HMAC.inttobytes(kem, 2);
            var KEM =ctx.ECDH.asciitobytes(txt);
            var SUITE_ID = [];
            var k=KEM.length;
            for (var i=0;i<k;i++)
                SUITE_ID[i]=KEM[i];
            SUITE_ID[k]=KEM_ID[0];
            SUITE_ID[k+1]=KEM_ID[1];

            var PRK=this.LabeledExtract(null,SUITE_ID,"dkp_prk",SEED);
            var S;
		    if (kem==32 || kem==33) {  // RFC7748
                S=this.LabeledExpand(PRK,SUITE_ID,"sk",null,ctx.ECDH.EGS);
                this.reverse(S);
                if (kem==32)
                {
	                S[ctx.ECDH.EGS-1]&=248;
				    S[0]&=127;
				    S[0]|=64;
                } else {
				    S[ctx.ECDH.EGS-1]&=252;
				    S[0]|=128;
                }
            } else {
                var bit_mask;
                if (kem==18) bit_mask=1;
                else bit_mask=0xff;
                S=[];
                for (var i=0;i<ctx.ECDH.EGS;i++)
                    S[i]=0;
                while (!ctx.ECDH.IN_RANGE(S) && counter<256)
                {
                    INFO[0]=counter;
                    S=this.LabeledExpand(PRK,SUITE_ID,"candidate",INFO,ctx.ECDH.EGS);
                    S[0]&=bit_mask;
                    counter++;
                }
            }
            for (var i=0;i<ctx.ECDH.EGS;i++)
                SK[i]=S[i];
	        ctx.ECDH.KEY_PAIR_GENERATE(null, SK, PK);  
            if (kem==32 || kem==33)
                this.reverse(PK);
            if (counter<256) return true;
            return false;
        },

        encap: function(config_id,skE,pkE,pkR) {
            var DH = [];
            var KEMCONTEXT=[];
	        var kem = config_id&255;
	        
		    if (kem==32 || kem==33) {
			    this.reverse(pkR);
	            ctx.ECDH.ECPSVDP_DH(skE, pkR, DH, 0);
		        this.reverse(pkR);
		        this.reverse(DH);
	        } else {
	            ctx.ECDH.ECPSVDP_DH(skE, pkR, DH, 0);
            }
            var k=0;
            for (var i=0;i<pkE.length;i++)
                KEMCONTEXT[k++]=pkE[i];
            for (var i=0;i<pkR.length;i++)
                KEMCONTEXT[k++]=pkR[i];

    	    return this.ExtractAndExpand(config_id,DH,KEMCONTEXT);
        },

        decap: function(config_id,skR,pkE,pkR) {
            var DH = [];
            var KEMCONTEXT=[];
	        var kem = config_id&255;


	        if (kem==32 || kem==33) {
		        this.reverse(pkE);
	            ctx.ECDH.ECPSVDP_DH(skR, pkE, DH,0);
		        this.reverse(pkE);
		        this.reverse(DH);
	        } else{ 
                ctx.ECDH.ECPSVDP_DH(skR, pkE, DH, 0);
            }

            var k=0;
            for (var i=0;i<pkE.length;i++)
                KEMCONTEXT[k++]=pkE[i];
            for (var i=0;i<pkR.length;i++)
                KEMCONTEXT[k++]=pkR[i];

    	    return this.ExtractAndExpand(config_id,DH,KEMCONTEXT);
        },

        authEncap: function(config_id,skE,skS,pkE,pkR,pkS) {
            var pklen=pkE.length;
            var DH = [];
            var DH1 = [];
            var KEMCONTEXT = [];
	
	        var kem = config_id&255;

		    if (kem==32 || kem==33) {
			    this.reverse(pkR);
	            ctx.ECDH.ECPSVDP_DH(skE, pkR, DH,0);
	            ctx.ECDH.ECPSVDP_DH(skS, pkR, DH1,0);
                this.reverse(pkR);
		        this.reverse(DH);
		        this.reverse(DH1);
	        } else {
	            ctx.ECDH.ECPSVDP_DH(skE, pkR, DH,0);
	            ctx.ECDH.ECPSVDP_DH(skS, pkR, DH1,0);
            }
	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = DH[i];
                ZZ[ctx.ECDH.EFS+i]= DH1[i];
	        }

	        for (var i=0;i<pklen;i++) {
		        KEMCONTEXT[i] = pkE[i];
                KEMCONTEXT[pklen+i]= pkR[i];
                KEMCONTEXT[2*pklen+i]= pkS[i];
	        }

	        return this.ExtractAndExpand(config_id,ZZ,KEMCONTEXT);
        },

        authDecap: function(config_id,skR,pkE,pkR,pkS) {
            var pklen=pkE.length;
            var DH = [];
            var DH1 = [];
            var KEMCONTEXT = [];

		    var kem = config_id&255;
	
	        if (kem==32 || kem==33) {
		        this.reverse(pkE);
		        this.reverse(pkS);
	            ctx.ECDH.ECPSVDP_DH(skR, pkE, DH,0);
	            ctx.ECDH.ECPSVDP_DH(skR, pkS, DH1,0);
		        this.reverse(pkE);
		        this.reverse(pkS);
		        this.reverse(DH);
		        this.reverse(DH1);
	        } else {
	            ctx.ECDH.ECPSVDP_DH(skR, pkE, DH,0);
	            ctx.ECDH.ECPSVDP_DH(skR, pkS, DH1,0);
            }    

	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = DH[i];
                ZZ[ctx.ECDH.EFS+i]= DH1[i];
	        }
            
	        for (var i=0;i<pklen;i++) {
		        KEMCONTEXT[i] = pkE[i];
                KEMCONTEXT[pklen+i]= pkR[i];
                KEMCONTEXT[2*pklen+i]= pkS[i];
	        }

	        return this.ExtractAndExpand(config_id,ZZ,KEMCONTEXT);
        },

        keySchedule: function(config_id,key,nonce,exp_secret,mode,Z,info,psk,pskID) {
	        var context = [];
	        var kem=config_id&255;
	        var kdf=(config_id>>8)&3;
	        var aead=(config_id>>10)&3;
            var num,k;
            var txt="HPKE"; 
            var KEM =ctx.ECDH.asciitobytes(txt);
            var SUITE_ID = [];
            k=KEM.length;
            for (var i=0;i<k;i++)
                SUITE_ID[i]=KEM[i];
            num = ctx.HMAC.inttobytes(kem, 2);
            SUITE_ID[k++]=num[0];
            SUITE_ID[k++]=num[1];
            num = ctx.HMAC.inttobytes(kdf, 2);
            SUITE_ID[k++]=num[0];
            SUITE_ID[k++]=num[1];
            num = ctx.HMAC.inttobytes(aead, 2);
            SUITE_ID[k++]=num[0];
            SUITE_ID[k++]=num[1];
            k=0;
	        var ar = ctx.HMAC.inttobytes(mode, 1);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

            var H=this.LabeledExtract(null,SUITE_ID,"psk_id_hash",pskID);
            for (var i=0;i<ctx.ECP.HASH_TYPE;i++)
                context[k++]=H[i];
            H=this.LabeledExtract(null,SUITE_ID,"info_hash",info);
            for (var i=0;i<ctx.ECP.HASH_TYPE;i++)
                context[k++]=H[i];

            //H=this.LabeledExtract(null,SUITE_ID,"psk_hash",psk);
            //var secret=this.LabeledExtract(H,SUITE_ID,"secret",Z);

            var secret=this.LabeledExtract(Z,SUITE_ID,"secret",psk);

            var ex=this.LabeledExpand(secret,SUITE_ID,"key",context,ctx.ECP.AESKEY);
            for (var i=0;i<ex.length;i++) key[i]=ex[i];

            ex=this.LabeledExpand(secret,SUITE_ID,"base_nonce",context,12);
            for (var i=0;i<ex.length;i++) nonce[i]=ex[i];

            if (exp_secret!=null)
            {
                ex=this.LabeledExpand(secret,SUITE_ID,"exp",context,ctx.ECP.HASH_TYPE);
                for (var i=0;i<ex.length;i++) exp_secret[i]=ex[i];
            }
        }

    };
    return HPKE;
};

if (typeof module !== "undefined" && typeof module.exports !== "undefined") {
    module.exports = {
        HPKE: HPKE
    };
}

