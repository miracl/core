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

package XXX

//import "fmt"
import "github.com/miracl/core/go/core"

func reverse(X []byte) {
	lx:=len(X)
	for i:=0;i<lx/2;i++ {
		ch :=X[i]
		X[i]=X[lx-i-1]
		X[lx-i-1]=ch
	}
}


func labeledExtract(SALT []byte,label string,IKM []byte) []byte {
	rfc:="RFCXX"+"XX "
	prefix:=[]byte(rfc+label)
	var LIKM []byte
	for i:=0;i<len(prefix);i++ {
		LIKM=append(LIKM,prefix[i])
	}
	if (IKM!=nil) {
		for i:=0;i<len(IKM);i++ {
			LIKM=append(LIKM,IKM[i])
		}
	}
	return core.HKDF_Extract(core.MC_SHA2,HASH_TYPE,SALT,LIKM);
}

func labeledExpand(PRK []byte,label string,INFO []byte,L int) []byte {
	rfc:="RFCXX" +"XX "
	prefix:=[]byte(rfc+label)
	ar := core.InttoBytes(L,2)
	var LINFO []byte
	for i:=0;i<len(ar);i++ {
		LINFO = append(LINFO, ar[i])
	}
	for i:=0;i<len(prefix);i++ {
		LINFO=append(LINFO,prefix[i])
	}	
	for i:=0;i<len(INFO);i++ {
		LINFO=append(LINFO,INFO[i])
	}	

	return core.HKDF_Expand(core.MC_SHA2,HASH_TYPE,L,PRK,LINFO)
}

func extractAndExpand(DH []byte,context []byte) []byte {
	PRK:=labeledExtract(nil,"dh",DH);
	return labeledExpand(PRK,"prk",context,HASH_TYPE);
}

func Encap(config_id int,SKE []byte,pkE []byte,pkR []byte) []byte {
	var skE [EGS]byte
	var DH [EFS]byte
	var kemcontext [] byte
	kem := config_id&255

	for i:=0;i<EGS;i++ {
		skE[i]=SKE[i]
	}
	if kem==32 || kem==33 {
		reverse(skE[:])
		if kem==32 {
			skE[EGS-1]&=248
			skE[0]&=127
			skE[0]|=64
		} else {
			skE[EGS-1]&=252
			skE[0]|=128
		}
	}
	
	ECDH_KEY_PAIR_GENERATE(nil, skE[:], pkE)
	if kem==32 || kem==33 {
		reverse(pkR)
	}
	ECDH_ECPSVDP_DH(skE[:], pkR[:], DH[:])
	if kem==32 || kem==33 {
		reverse(pkR)
		reverse(pkE)
		reverse(DH[:])
	}
	for i:=0;i<len(pkE);i++ {
		kemcontext=append(kemcontext,pkE[i]);
	}
	for i:=0;i<len(pkR);i++ {
		kemcontext=append(kemcontext,pkR[i]);
	}

	return extractAndExpand(DH[:],kemcontext)
}
		
func Decap(config_id int,pkE []byte,SKR []byte) []byte {
	var skR [EGS]byte
	var DH [EFS]byte
	pkR:=make([]byte,len(pkE)) 
	var kemcontext [] byte
	kem := config_id&255
	for i:=0;i<EGS;i++ {
		skR[i]=SKR[i]
	}
	if kem==32 || kem==33 {
		reverse(skR[:])
		reverse(pkE)
		if kem==32 {
			skR[EGS-1]&=248
			skR[0]&=127
			skR[0]|=64
		} else {
			skR[EGS-1]&=252
			skR[0]|=128
		}
	}
	ECDH_ECPSVDP_DH(skR[:], pkE, DH[:])
	if kem==32 || kem==33 {
		reverse(pkE)
		reverse(DH[:])
	}
	ECDH_KEY_PAIR_GENERATE(nil,skR[:],pkR)
	if kem==32 || kem==33 {
		reverse(pkR[:])
	}
	for i:=0;i<len(pkE);i++ {
		kemcontext=append(kemcontext,pkE[i]);
	}
	for i:=0;i<len(pkR);i++ {
		kemcontext=append(kemcontext,pkR[i]);
	}

	return extractAndExpand(DH[:],kemcontext)
}

func AuthEncap(config_id int,SKE []byte,pkE []byte,pkR []byte,SKS []byte) []byte {
	var skE [EGS]byte	
	var skS [EGS]byte
	var DH [EFS]byte
	var DH1 [EFS]byte
	pklen:=len(pkE)
	pkS:=make([]byte,pklen) 
	kemcontext:=make([] byte,3*pklen)
	kem := config_id&255

	for i:=0;i<EGS;i++ {
		skE[i]=SKE[i]
		skS[i]=SKS[i]
	}	
	if kem==32 || kem==33 {
		reverse(skE[:])
		reverse(skS[:])
		if kem==32 {
			skE[EGS-1]&=248
			skE[0]&=127
			skE[0]|=64
			skS[EGS-1]&=248
			skS[0]&=127
			skS[0]|=64	
		} else {
			skE[EGS-1]&=252
			skE[0]|=128
			skS[EGS-1]&=252
			skS[0]|=128
		}
	}
	
	ECDH_KEY_PAIR_GENERATE(nil, skE[:], pkE)
	if kem==32 || kem==33 {
		reverse(pkR)
	}
	ECDH_ECPSVDP_DH(skE[:], pkR, DH[:])
	ECDH_ECPSVDP_DH(skS[:], pkR, DH1[:])
	if kem==32 || kem==33 {
		reverse(DH[:])
		reverse(DH1[:])
	}
	var ZZ [2*EFS]byte
	for i:=0;i<EFS;i++ {
		ZZ[i]=DH[i];
		ZZ[EFS+i]=DH1[i];
	}

	ECDH_KEY_PAIR_GENERATE(nil, skS[:], pkS[:])
	
	if kem==32 || kem==33 {
		reverse(pkR)
		reverse(pkE)
		reverse(pkS[:])
	}
	for i:=0;i<pklen;i++ {
		kemcontext[i] = pkE[i]
        kemcontext[pklen+i]= pkR[i]
        kemcontext[2*pklen+i]= pkS[i]
	}
	return extractAndExpand(ZZ[:],kemcontext)
}

func AuthDecap(config_id int,pkE []byte,SKR []byte,pkS []byte) []byte  {
	var skR [EGS]byte	
	var DH [EFS]byte
	var DH1 [EFS]byte
	pklen:=len(pkE)
	pkR:=make([]byte,pklen) 
	kemcontext:=make([] byte,3*pklen)

	kem := config_id&255

	for i:=0;i<EGS;i++ {
		skR[i]=SKR[i]
	}
	if kem==32 || kem==33 {
		reverse(skR[:])
		reverse(pkE)
		reverse(pkS)
		if kem==32 {
			skR[EGS-1]&=248
			skR[0]&=127
			skR[0]|=64
		} else {
			skR[EGS-1]&=252
			skR[0]|=128
		}
	}
	ECDH_ECPSVDP_DH(skR[:], pkE, DH[:])
	ECDH_ECPSVDP_DH(skR[:], pkS, DH1[:])
	if kem==32 || kem==33 {
		reverse(DH[:])
		reverse(DH1[:])
	}
	var ZZ [2*EFS]byte
	for i:=0;i<EFS;i++ {
		ZZ[i]=DH[i];
		ZZ[EFS+i]=DH1[i];
	}

	ECDH_KEY_PAIR_GENERATE(nil, skR[:], pkR[:])

	if kem==32 || kem==33 {
		reverse(pkR[:])
		reverse(pkE)
		reverse(pkS)
	}
	for i:=0;i<pklen;i++ {
		kemcontext[i] = pkE[i]
        kemcontext[pklen+i]= pkR[i]
        kemcontext[2*pklen+i]= pkS[i]
	}
	return extractAndExpand(ZZ[:],kemcontext)
}

/*
func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}
*/

func KeySchedule(config_id int,mode int,Z []byte,info []byte,psk []byte,pskID []byte) ([]byte,[]byte,[]byte) {
	var context []byte

	kem_id:=config_id&255
	kdf_id:=(config_id>>8)&3
	aead_id:=(config_id>>10)&3

	ar:=core.InttoBytes(kem_id, 2)
	for i:=0;i<len(ar);i++ {
		context = append(context, ar[i])
	}
	ar=core.InttoBytes(kdf_id, 2)
	for i:=0;i<len(ar);i++ {
		context = append(context, ar[i])
	}
	ar=core.InttoBytes(aead_id, 2)
	for i:=0;i<len(ar);i++ {
		context = append(context, ar[i])
	}
	ar = core.InttoBytes(mode, 1)
	for i:=0;i<len(ar);i++ {
		context = append(context, ar[i])
	}

	var ZEROS [HASH_TYPE]byte
	for i:=0;i<HASH_TYPE;i++ {
		ZEROS[i]=0
	}

	H:=labeledExtract(ZEROS[:],"pskID_hash",pskID);
	for i:=0;i<HASH_TYPE;i++ {
		context=append(context,H[i])
	}
	H=labeledExtract(ZEROS[:],"info",info)
	for i:=0;i<HASH_TYPE;i++ {
		context=append(context,H[i])
	}

	if psk==nil {
		H=labeledExtract(ZEROS[:],"psk_hash",ZEROS[:])
	} else {
		H=labeledExtract(ZEROS[:],"psk_hash",psk)
	}
	secret:=labeledExtract(H,"zz",Z)

	key:=labeledExpand(secret,"key",context,AESKEY)
	nonce:=labeledExpand(secret,"nonce",context,12)
	exp_secret:=labeledExpand(secret,"exp",context,HASH_TYPE)

	return key,nonce,exp_secret
}	

