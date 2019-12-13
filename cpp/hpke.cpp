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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hpke_ZZZ.h"

void ZZZ::HPKE_Encap(int config_id,csprng *RNG,octet *SKE,octet *Z,octet *pkE,octet *pkR)
{
    int res,kem;
    char ske[EGS_ZZZ];
    octet skE={0,sizeof(ske),ske};

    kem=config_id&7;
    if (RNG==NULL)
    {
        OCT_copy(&skE,SKE);
        if (kem==2 || kem==4) {
            OCT_reverse(&skE);
            if (kem==2)
            {
                skE.val[31]&=248;  // x25519 scalar processing
                skE.val[0]&=127;
                skE.val[0]|=64;
            } else {
                skE.val[55]&=252;
                skE.val[0]|=128;
            }
        }
    }

    res=ECP_KEY_PAIR_GENERATE(RNG, &skE, pkE);
    if (kem==2 || kem==4)
        OCT_reverse(pkR);

    res=ECP_SVDP_DH(&skE, pkR, Z);
    if (kem==2 || kem==4) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(Z);
    }
}

void ZZZ::HPKE_Decap(int config_id,octet *Z,octet *pkE,octet *SKR)
{
    int res,kem;
    char skr[EGS_ZZZ];
    octet skR={0,sizeof(skr),skr};
    kem=config_id&7;
    OCT_copy(&skR,SKR);

    if (kem==2 || kem==4) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        if (kem==2)
        {
            skR.val[31]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
            } else {
                skR.val[55]&=252;
                skR.val[0]|=128;
            }       

    }
    ECP_SVDP_DH(&skR, pkE, Z);
    if (kem==2 || kem==4) {
        OCT_reverse(pkE);
        OCT_reverse(Z);
    }
}

void ZZZ::HPKE_AuthEncap(int config_id,csprng *RNG,octet *SKE,octet *Z,octet *pkE,octet *pkR,octet *SKI)
{
    int res,kem;
    char ske[EGS_ZZZ];
    octet skE={0,sizeof(ske),ske};
    char ski[EGS_ZZZ];
    octet skI={0,sizeof(ski),ski};
    char nz[EFS_ZZZ];
    octet NZ={0,sizeof(nz),nz};
    kem=config_id&7;

    if (RNG==NULL)
    {
        OCT_copy(&skE,SKE);
        OCT_copy(&skI,SKI);
        if (kem==2 || kem==4) {
            OCT_reverse(&skE);
            OCT_reverse(&skI);
            if (kem==2)
            {
                skE.val[31]&=248;
                skE.val[0]&=127;
                skE.val[0]|=64;
                skI.val[31]&=248;
                skI.val[0]&=127;
                skI.val[0]|=64;
            } else {
                skE.val[55]&=252;
                skE.val[0]|=128;
                skI.val[55]&=252;
                skI.val[0]|=128;
            }
        }
    }

    ECP_KEY_PAIR_GENERATE(RNG, &skE, pkE);
    if (kem==2 || kem==4)
        OCT_reverse(pkR);

    ECP_SVDP_DH(&skE, pkR, Z);
    ECP_SVDP_DH(&skI, pkR, &NZ);
    if (kem==2 || kem==4) {
        OCT_reverse(pkR);
        OCT_reverse(pkE);
        OCT_reverse(Z);
        OCT_reverse(&NZ);
    }
    OCT_joctet(Z,&NZ);
}

void ZZZ::HPKE_AuthDecap(int config_id,octet *Z,octet *pkE,octet *SKR,octet *pkI)
{
    int res,kem;
    char skr[EGS_ZZZ];
    octet skR={0,sizeof(skr),skr};
    char nz[EFS_ZZZ];
    octet NZ={0,sizeof(nz),nz};
    OCT_copy(&skR,SKR);
    kem=config_id&7;

    if (kem==2 || kem==4) {
        OCT_reverse(&skR);
        OCT_reverse(pkE);
        OCT_reverse(pkI);
        if (kem==2)
        {
            skR.val[31]&=248;
            skR.val[0]&=127;
            skR.val[0]|=64;
        } else {
            skR.val[55]&=252;
            skR.val[0]|=128;
        }

    }

    ECP_SVDP_DH(&skR, pkE, Z);
    ECP_SVDP_DH(&skR, pkI, &NZ);
    if (kem==2 || kem==4) {
        OCT_reverse(pkE);
        OCT_reverse(pkI);
        OCT_reverse(Z);
        OCT_reverse(&NZ);
    }
    OCT_joctet(Z,&NZ);
}

void ZZZ::HPKE_KeySchedule(int config_id,octet *key,octet *nonce,int mode,octet *pkR,octet *Z,octet *pkE,octet *info,octet *psk,octet *pskID,octet *pkI)
{
    char context[550];
    octet CONTEXT={0,sizeof(context),context};
    char h[64];
    octet H={0,sizeof(h),h};
    char secret[64];
    octet SECRET={0,sizeof(secret),secret};
    char full_context[550];
    octet FULL_CONTEXT={0,sizeof(full_context),full_context};

    int kem_id=config_id&7;
    int kdf_id=(config_id>>3)&3;
    int aead_id=(config_id>>5)&3;

    OCT_jint(&CONTEXT,mode,1);
    OCT_jint(&CONTEXT,kem_id,2);
    OCT_jint(&CONTEXT,kdf_id,2);
    OCT_jint(&CONTEXT,aead_id,2);
    OCT_joctet(&CONTEXT,pkE);
    OCT_joctet(&CONTEXT,pkR);
    if (pkI==NULL)
        OCT_jbyte(&CONTEXT,0,pkR->max);
    else
        OCT_joctet(&CONTEXT,pkI);

    SPhash(MC_SHA2,HASH_TYPE_ZZZ,&H,pskID);
    OCT_joctet(&CONTEXT,&H);
    SPhash(MC_SHA2,HASH_TYPE_ZZZ,&H,info);
    OCT_joctet(&CONTEXT,&H);

    //printf("Context= "); OCT_output(&CONTEXT);
    HKDF_Extract(MC_SHA2,HASH_TYPE_ZZZ,&SECRET,psk,Z);
    //printf("Secret= "); OCT_output(&SECRET);

    OCT_jbytes(&FULL_CONTEXT,(char *)"hpke key",8); OCT_joctet(&FULL_CONTEXT,&CONTEXT);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,key,AESKEY_ZZZ,&SECRET,&FULL_CONTEXT);
    OCT_clear(&FULL_CONTEXT);
    OCT_jbytes(&FULL_CONTEXT,(char *)"hpke nonce",10); OCT_joctet(&FULL_CONTEXT,&CONTEXT);
    HKDF_Expand(MC_SHA2,HASH_TYPE_ZZZ,nonce,12,&SECRET,&FULL_CONTEXT);
}
