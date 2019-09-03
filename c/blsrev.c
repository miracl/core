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

/* Boneh-Lynn-Shacham signature 128-bit API */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bls_ZZZ.h"


//static FP4_YYY G2_TAB[G2_TABLE_ZZZ];  // space for precomputation on fixed G2 parameter

/* hash a message to an ECP point, using SHA3 */

static void BLS_HASHIT(ECP2_ZZZ *P, char *m)
{
    int i;
    sha3 hs;
    char h[MODBYTES_XXX];
    octet HM = {0, sizeof(h), h};
    SHA3_init(&hs, SHAKE256);
    for (i = 0; m[i] != 0; i++) SHA3_process(&hs, m[i]);
    SHA3_shake(&hs, HM.val, MODBYTES_XXX);
    HM.len = MODBYTES_XXX;
    ECP2_ZZZ_mapit(P, &HM);
}

int BLS_ZZZ_INIT()
{
//    ECP2_ZZZ G;
//    if (!ECP2_ZZZ_generator(&G)) return BLS_FAIL;
//    PAIR_ZZZ_precomp(G2_TAB, &G);  // should be done just once on start-up
    return BLS_OK;
}

/* generate key pair, private key S, public key W */

int BLS_ZZZ_KEY_PAIR_GENERATE(csprng *RNG, octet* S, octet *W)
{
    ECP_ZZZ G;
    BIG_XXX s, q;
    BIG_XXX_rcopy(q, CURVE_Order_ZZZ);

    if (!ECP_ZZZ_generator(&G)) return BLS_FAIL;
    BIG_XXX_randtrunc(s, q, 2 * CURVE_SECURITY_ZZZ, RNG);
    BIG_XXX_toBytes(S->val, s);
    S->len = MODBYTES_XXX;
    PAIR_ZZZ_G1mul(&G, s);

    ECP_ZZZ_toOctet(W, &G, true);
    return BLS_OK;
}

/* Sign message m using private key S to produce signature SIG */

int BLS_ZZZ_SIGN(octet *SIG, char *m, octet *S)
{
    BIG_XXX s;
    ECP2_ZZZ D;
    BLS_HASHIT(&D, m);
    BIG_XXX_fromBytes(s, S->val);
    PAIR_ZZZ_G2mul(&D, s);

    ECP2_ZZZ_toOctet(SIG, &D, true); /* compress output */
    return BLS_OK;
}

/* Verify signature of message m, the signature SIG, and the public key W */

int BLS_ZZZ_VERIFY(octet *SIG, char *m, octet *W)
{
    FP12_YYY v;
    ECP_ZZZ G, PK;
    ECP2_ZZZ D, HM;
    BLS_HASHIT(&HM, m);
    
	ECP2_ZZZ_fromOctet(&D, SIG);
	if (!PAIR_ZZZ_G2member(&D)) return BLS_FAIL;
    ECP2_ZZZ_neg(&D);

    ECP_ZZZ_fromOctet(&PK, W);

// Use new multi-pairing mechanism
    if (!ECP_ZZZ_generator(&G)) return BLS_FAIL;
    FP12_YYY r[ATE_BITS_ZZZ];
    PAIR_ZZZ_initmp(r);
    PAIR_ZZZ_another(r, &D, &G);
    PAIR_ZZZ_another(r, &HM, &PK);
    PAIR_ZZZ_miller(&v, r);

//.. or alternatively
//      if (!ECP2_ZZZ_generator(&G)) return BLS_FAIL;
//      PAIR_ZZZ_double_ate(&v,&G,&D,&PK,&HM);

    PAIR_ZZZ_fexp(&v);
    if (FP12_YYY_isunity(&v)) return BLS_OK;
    return BLS_FAIL;
}

