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

func Encap(config_id int,RNG *core.RAND,SKE []byte,pkE []byte,pkR []byte) []byte {
	var skE [EGS]byte
	var Z [EFS]byte
	kem := config_id&7
	if RNG == nil {
		for i:=0;i<EGS;i++ {
			skE[i]=SKE[i]
		}
		if kem==2 || kem==4 {
			reverse(skE[:])
			if kem==2 {
				skE[EGS-1]&=248
				skE[0]&=127
				skE[0]|=64
			} else {
				skE[EGS-1]&=252
				skE[0]|=128
			}
		}
	}
	ECDH_KEY_PAIR_GENERATE(RNG, skE[:], pkE)
	if kem==2 || kem==4 {
		reverse(pkR)
	}
	ECDH_ECPSVDP_DH(skE[:], pkR, Z[:])
	if kem==2 || kem==4 {
		reverse(pkR)
		reverse(pkE)
		reverse(Z[:])
	}
	return Z[:]
}
		
func Decap(config_id int,pkE []byte,SKR []byte) []byte {
	var skR [EGS]byte
	var Z [EFS]byte
	kem := config_id&7
	for i:=0;i<EGS;i++ {
		skR[i]=SKR[i]
	}
	if kem==2 || kem==4 {
		reverse(skR[:])
		reverse(pkE)
		if kem==2 {
			skR[EGS-1]&=248
			skR[0]&=127
			skR[0]|=64
		} else {
			skR[EGS-1]&=252
			skR[0]|=128
		}
	}
	ECDH_ECPSVDP_DH(skR[:], pkE, Z[:])
	if kem==2 || kem==4 {
		reverse(pkE)
		reverse(Z[:])
	}
	return Z[:]
}

func AuthEncap(config_id int,RNG *core.RAND,SKE []byte,pkE []byte,pkR []byte,SKI []byte) []byte {
	var skE [EGS]byte	
	var skI [EGS]byte
	var Z [EFS]byte
	var NZ [EFS]byte
	kem := config_id&7

	if RNG == nil {
		for i:=0;i<EGS;i++ {
			skE[i]=SKE[i]
			skI[i]=SKI[i]
		}	
		if kem==2 || kem==4 {
			reverse(skE[:])
			reverse(skI[:])
			if kem==2 {
				skE[EGS-1]&=248
				skE[0]&=127
				skE[0]|=64
				skI[EGS-1]&=248
				skI[0]&=127
				skI[0]|=64	
			} else {
				skE[EGS-1]&=252
				skE[0]|=128
				skI[EGS-1]&=252
				skI[0]|=128
			}
		}
	}
	ECDH_KEY_PAIR_GENERATE(RNG, skE[:], pkE)
	if kem==2 || kem==4 {
		reverse(pkR)
	}
	ECDH_ECPSVDP_DH(skE[:], pkR, Z[:])
	ECDH_ECPSVDP_DH(skI[:], pkR, NZ[:])
	if kem==2 || kem==4 {
		reverse(pkR)
		reverse(pkE)
		reverse(Z[:])
		reverse(NZ[:])
	}
	var ZZ []byte
	for i:=0;i<EFS;i++ {
		ZZ = append(ZZ,Z[i])
	}
	for i:=0;i<EFS;i++ {
		ZZ = append(ZZ,NZ[i])
	}
	return ZZ
}

func AuthDecap(config_id int,pkE []byte,SKR []byte,pkI []byte) []byte  {
	var skR [EGS]byte	
	var Z [EFS]byte
	var NZ [EFS]byte
	kem := config_id&7
	for i:=0;i<EGS;i++ {
		skR[i]=SKR[i]
	}
	if kem==2 || kem==4 {
		reverse(skR[:])
		reverse(pkE)
		reverse(pkI)
		if kem==2 {
			skR[EGS-1]&=248
			skR[0]&=127
			skR[0]|=64
		} else {
			skR[EGS-1]&=252
			skR[0]|=128
		}
	}
	ECDH_ECPSVDP_DH(skR[:], pkE, Z[:])
	ECDH_ECPSVDP_DH(skR[:], pkI, NZ[:])
	if kem==2 || kem==4 {
		reverse(pkI)
		reverse(pkE)
		reverse(Z[:])
		reverse(NZ[:])
	}
	var ZZ []byte
	for i:=0;i<EFS;i++ {
		ZZ=append(ZZ,Z[i])
	}
	for i:=0;i<EFS;i++ {
		ZZ=append(ZZ,NZ[i])
	}
	return ZZ
}

/*
func printBinary(array []byte) {
	for i := 0; i < len(array); i++ {
		fmt.Printf("%02x", array[i])
	}
	fmt.Printf("\n")
}
*/

func KeySchedule(config_id int,mode int,pkR []byte,Z []byte,pkE []byte,info []byte,psk []byte,pskID []byte,pkI []byte) ([]byte,[]byte) {
	var CONTEXT []byte
	var FULL_CONTEXT[]byte

	kem_id:=config_id&7
	kdf_id:=(config_id>>3)&3
	aead_id:=(config_id>>5)&3

	ar := core.InttoBytes(mode, 1)
	for i:=0;i<len(ar);i++ {
		CONTEXT = append(CONTEXT, ar[i])
	}
	ar=core.InttoBytes(kem_id, 2)
	for i:=0;i<len(ar);i++ {
		CONTEXT = append(CONTEXT, ar[i])
	}
	ar=core.InttoBytes(kdf_id, 2)
	for i:=0;i<len(ar);i++ {
		CONTEXT = append(CONTEXT, ar[i])
	}
	ar=core.InttoBytes(aead_id, 2)
	for i:=0;i<len(ar);i++ {
		CONTEXT = append(CONTEXT, ar[i])
	}
	for i:=0;i<len(pkE);i++ {
		CONTEXT = append(CONTEXT, pkE[i])
	}
	for i:=0;i<len(pkR);i++ {
		CONTEXT = append(CONTEXT, pkR[i])
	}
	if pkI==nil {
		for i:=0;i<len(pkR);i++ {
			CONTEXT = append(CONTEXT, 0)
		}
	} else {
		for i:=0;i<len(pkI);i++ {
			CONTEXT = append(CONTEXT, pkI[i])
		}
	}
	H:=core.SPhashit(core.MC_SHA2,HASH_TYPE,pskID)
	for i:=0;i<HASH_TYPE;i++ {
		CONTEXT = append(CONTEXT, H[i])
	}
	H=core.SPhashit(core.MC_SHA2,HASH_TYPE,info)
	for i:=0;i<HASH_TYPE;i++ {
		CONTEXT = append(CONTEXT, H[i])
	}

	//fmt.Printf("Context = "); printBinary(CONTEXT[:])	
	SECRET:=core.HKDF_Extract(core.MC_SHA2,HASH_TYPE,psk,Z)
	//fmt.Printf("Secret =  "); printBinary(SECRET[:])
	
	pp:="hpke key"
	ar=[]byte(pp)
	for i:=0;i<len(ar);i++ {
		FULL_CONTEXT = append(FULL_CONTEXT, ar[i])
	}
	for i:=0;i<len(CONTEXT);i++ {
		FULL_CONTEXT = append(FULL_CONTEXT, CONTEXT[i])
	}
	key:=core.HKDF_Expand(core.MC_SHA2,HASH_TYPE,AESKEY,SECRET[:],FULL_CONTEXT)

	FULL_CONTEXT=nil
	pp="hpke nonce"
	ar=[]byte(pp)
	for i:=0;i<len(ar);i++ {
		FULL_CONTEXT = append(FULL_CONTEXT, ar[i])
	}
	for i:=0;i<len(CONTEXT);i++ {
		FULL_CONTEXT = append(FULL_CONTEXT, CONTEXT[i])
	}
	nonce:=core.HKDF_Expand(core.MC_SHA2,HASH_TYPE,12,SECRET[:],FULL_CONTEXT)

	return key,nonce
}	

