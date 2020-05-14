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

        LabeledExtract: function(SALT,label,IKM) {
            var rfc="RFCXX"+"XX ";
            var prefix=ctx.ECDH.asciitobytes(rfc+label);
            var IKMlen;
            if (IKM==null)
                IKMlen=0;
            else
                IKMlen=IKM.length;
            var LIKM = [];
            var k=0;
            for (var i=0;i<prefix.length;i++)
                LIKM[k++]=prefix[i];
            if (IKM!=null) {
                for (var i=0;i<IKM.length;i++)
                    LIKM[k++]=IKM[i];
            }

            return ctx.HMAC.HKDF_Extract(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,SALT,LIKM);
        },

        LabeledExpand: function(PRK,label,INFO,L) {
            var ar = ctx.HMAC.inttobytes(L, 2);
            var rfc="RFCXX" +"XX ";
            var prefix=ctx.ECDH.asciitobytes(rfc+label);   
            var LINFO = [];
            LINFO[0]=ar[0];
            LINFO[1]=ar[1];
            var k=2;
            for (var i=0;i<prefix.length;i++)
                LINFO[k++]=prefix[i];
            for (var i=0;i<INFO.length;i++)
                LINFO[k++]=INFO[i];

            return ctx.HMAC.HKDF_Expand(ctx.HMAC.MC_SHA2,ctx.ECP.HASH_TYPE,L,PRK,LINFO);
        },

        ExtractAndExpand: function(DH,CONTEXT) {
            var PRK=this.LabeledExtract(null,"dh",DH);
            return this.LabeledExpand(PRK,"prk",CONTEXT,ctx.ECP.HASH_TYPE);
        },

        encap: function(config_id,SKE,pkE,pkR) {
            var skE = [];
            var DH = [];
            var KEMCONTEXT=[];
	        var kem = config_id&255;
	        
		    for (var i=0;i<ctx.ECDH.EGS;i++) 
			    skE[i]=SKE[i];
		    if (kem==32 || kem==33) {
			    this.reverse(skE);
			    if (kem==32) {
				    skE[ctx.ECDH.EGS-1]&=248;
				    skE[0]&=127;
				    skE[0]|=64;
			    } else {
				    skE[ctx.ECDH.EGS-1]&=252;
				    skE[0]|=128;
			    }
		    }
	        
	        ctx.ECDH.KEY_PAIR_GENERATE(null, skE, pkE);
	        if (kem==32 || kem==33) 
		        this.reverse(pkR);

	        ctx.ECDH.ECPSVDP_DH(skE, pkR, DH);
	        if (kem==32 || kem==33) {
		        this.reverse(pkR);
		        this.reverse(pkE);
		        this.reverse(DH);
	        }
            var k=0;
            for (var i=0;i<pkE.length;i++)
                KEMCONTEXT[k++]=pkE[i];
            for (var i=0;i<pkR.length;i++)
                KEMCONTEXT[k++]=pkR[i];

    	    return this.ExtractAndExpand(DH,KEMCONTEXT);
        },

        decap: function(config_id,pkE,SKR) {
            var skR = [];
            var DH = [];
            var KEMCONTEXT=[];
            var pkR = [];
	        var kem = config_id&255;

	        for (var i=0;i<ctx.ECDH.EGS;i++)
		        skR[i]=SKR[i];
	        if (kem==32 || kem==33) {
		        this.reverse(skR);
		        this.reverse(pkE);
		        if (kem==32) {
			        skR[ctx.ECDH.EGS-1]&=248;
			        skR[0]&=127;
			        skR[0]|=64;
		        } else {
			        skR[ctx.ECDH.EGS-1]&=252;
			        skR[0]|=128;
		        }
	        }
	        ctx.ECDH.ECPSVDP_DH(skR, pkE, DH);
	        if (kem==32 || kem==33) {
		        this.reverse(pkE);
		        this.reverse(DH);
	        }
            ctx.ECDH.KEY_PAIR_GENERATE(null,skR,pkR);
	        if (kem==32 || kem==33) {
		        this.reverse(pkR);
	        }
            var k=0;
            for (var i=0;i<pkE.length;i++)
                KEMCONTEXT[k++]=pkE[i];
            for (var i=0;i<pkR.length;i++)
                KEMCONTEXT[k++]=pkR[i];

    	    return this.ExtractAndExpand(DH,KEMCONTEXT);
        },

        authEncap: function(config_id,SKE,pkE,pkR,SKS) {
            var pklen=pkE.length;
            var skE = [];
            var DH = [];
            var DH1 = [];
            var skS = [];
            var pkS = [];
            var KEMCONTEXT = [];
	
	        var kem = config_id&255;

		    for (var i=0;i<ctx.ECDH.EGS;i++) {
			    skE[i]=SKE[i];
			    skS[i]=SKS[i];
		    }	
		    if (kem==32 || kem==33) {
			    this.reverse(skE);
			    this.reverse(skS);
			    if (kem==32) {
				    skE[ctx.ECDH.EGS-1]&=248;
				    skE[0]&=127;
				    skE[0]|=64;
				    skS[ctx.ECDH.EGS-1]&=248;
				    skS[0]&=127;
				    skS[0]|=64;	
			    } else {
				    skE[ctx.ECDH.EGS-1]&=252;
				    skE[0]|=128;
				    skS[ctx.ECDH.EGS-1]&=252;
				    skS[0]|=128;
			    }
		    }
	     
	        ctx.ECDH.KEY_PAIR_GENERATE(null, skE, pkE);
	        if (kem==32 || kem==33) 
		        this.reverse(pkR);
	
	        ctx.ECDH.ECPSVDP_DH(skE, pkR, DH);
	        ctx.ECDH.ECPSVDP_DH(skS, pkR, DH1);

	        if (kem==32 || kem==33) {
		        this.reverse(DH);
		        this.reverse(DH1);
	        }
	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = DH[i];
                ZZ[ctx.ECDH.EFS+i]= DH1[i];
	        }

            ctx.ECDH.KEY_PAIR_GENERATE(null, skS, pkS);

            if (kem==32 || kem==33)
            {
                this.reverse(pkR);
		        this.reverse(pkE);
                this.reverse(pkS);
            }
	        for (var i=0;i<pklen;i++) {
		        KEMCONTEXT[i] = pkE[i];
                KEMCONTEXT[pklen+i]= pkR[i];
                KEMCONTEXT[2*pklen+i]= pkS[i];
	        }

	        return this.ExtractAndExpand(ZZ,KEMCONTEXT);
        },

        authDecap: function(config_id,pkE,SKR,pkS) {
            var pklen=pkE.length;
            var skR = [];
            var DH = [];
            var DH1 = [];
            var pkR = [];
            var KEMCONTEXT = [];

		    var kem = config_id&255;

	        for (var i=0;i<ctx.ECDH.EGS;i++) 
		        skR[i]=SKR[i];
	
	        if (kem==32 || kem==33) {
		        this.reverse(skR);
		        this.reverse(pkE);
		        this.reverse(pkS);
		        if (kem==32) {
			        skR[ctx.ECDH.EGS-1]&=248;
			        skR[0]&=127;
			        skR[0]|=64;
		        } else {
		    	    skR[ctx.ECDH.EGS-1]&=252;
			        skR[0]|=128;
		        }  
	        }
	        ctx.ECDH.ECPSVDP_DH(skR, pkE, DH);
	        ctx.ECDH.ECPSVDP_DH(skR, pkS, DH1);

	        if (kem==32 || kem==33) {
		        this.reverse(DH);
		        this.reverse(DH1);
	        }
	        var ZZ = [];
	        for (var i=0;i<ctx.ECDH.EFS;i++) {
		        ZZ[i] = DH[i];
                ZZ[ctx.ECDH.EFS+i]= DH1[i];
	        }
            
            ctx.ECDH.KEY_PAIR_GENERATE(null, skR, pkR);

            if (kem==32 || kem==33)
            {
                this.reverse(pkR);
		        this.reverse(pkE);
                this.reverse(pkS);
            }
	        for (var i=0;i<pklen;i++) {
		        KEMCONTEXT[i] = pkE[i];
                KEMCONTEXT[pklen+i]= pkR[i];
                KEMCONTEXT[2*pklen+i]= pkS[i];
	        }

	        return this.ExtractAndExpand(ZZ,KEMCONTEXT);
        },

        keySchedule: function(config_id,key,nonce,exp_secret,mode,Z,info,psk,pskID) {
	        var context = [];
            var ZEROS = [];
	        var kem_id=config_id&255;
	        var kdf_id=(config_id>>8)&3;
	        var aead_id=(config_id>>10)&3;
            var k=0;
	
	        var ar=ctx.HMAC.inttobytes(kem_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        ar=ctx.HMAC.inttobytes(kdf_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        ar=ctx.HMAC.inttobytes(aead_id, 2);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

	        ar = ctx.HMAC.inttobytes(mode, 1);
            for (var i=0;i<ar.length;i++)
		        context[k++] = ar[i];

            for (var i=0;i<ctx.ECP.HASH_TYPE;i++)
                ZEROS[i]=0;

            var H=this.LabeledExtract(ZEROS,"pskID_hash",pskID);
            for (var i=0;i<ctx.ECP.HASH_TYPE;i++)
                context[k++]=H[i];
            H=this.LabeledExtract(ZEROS,"info",info);
            for (var i=0;i<ctx.ECP.HASH_TYPE;i++)
                context[k++]=H[i];

            if (psk==null)
                H=this.LabeledExtract(ZEROS,"psk_hash",ZEROS);
            else
                H=this.LabeledExtract(ZEROS,"psk_hash",psk);
            var secret=this.LabeledExtract(H,"zz",Z);

            var ex=this.LabeledExpand(secret,"key",context,ctx.ECP.AESKEY);
            for (var i=0;i<ex.length;i++) key[i]=ex[i];

            ex=this.LabeledExpand(secret,"nonce",context,12);
            for (var i=0;i<ex.length;i++) nonce[i]=ex[i];

            if (exp_secret!=null)
            {
                ex=this.LabeledExpand(secret,"exp",context,ctx.ECP.HASH_TYPE);
                for (var i=0;i<ex.length;i++) exp_secret[i]=ex[i];
            }
        }

    };
    return HPKE;
};
