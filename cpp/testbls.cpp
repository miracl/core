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

/* test driver and function exerciser for BLS Signature API Functions */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "bls_BN254.h"

#if CHUNK==32 || CHUNK==64
#include "bls_BLS12383.h"
#include "bls192_BLS24479.h"
#include "bls256_BLS48556.h"
#endif

#include "randapi.h"

/* To reverse the groups G1 and G2, edit BLS*.cpp

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

#define REVERSE here

See BLSREV*.cpp

*/

//#define REVERSE

using namespace core;

static char message[] = "This is a test message";

int bls_BN254(csprng *RNG)
{
    using namespace BN254;

    int i,res;
    char s[BGS_BN254];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BN254+1], sig[4 * BFS_BN254 + 1];        // w is 2* if not compressed else 1*. sig is 4* if not compressed, else 2*
#else
    char w[4 * BFS_BN254 + 1], sig[BFS_BN254 + 1];      // w is 4* if not compressed else 2*. sig is 2* if not compressed, else 1*
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};

    OCT_jstring(&M,message);

    res = BLS_INIT();
    if (res == BLS_FAIL)
    {
        printf("Failed to initialize\n");
        return res;
    }

    OCT_rand(&IKM,RNG,32);
    //IKM.len=32;
    //for (i=0;i<IKM.len;i++)
    //        IKM.val[i]=i+1;

    res = BLS_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        printf("Failed to generate keys\n");
        return res;
    }

    printf("Private key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);

    BLS_CORE_SIGN(&SIG, &M, &S);
    printf("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_CORE_VERIFY(&SIG, &M, &W);
    if (res == BLS_OK) printf("Signature is OK\n");
    else printf("Signature is *NOT* OK\n");
    return res;
}

#if CHUNK==32 || CHUNK==64

int bls_BLS12383(csprng *RNG)
{
    using namespace BLS12383;

    int i,res;
    char s[BGS_BLS12383];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BLS12383+1], sig[4 * BFS_BLS12383 + 1];
#else
    char w[4 * BFS_BLS12383 + 1], sig[BFS_BLS12383 + 1];
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};

    OCT_jstring(&M,message);

    res = BLS_INIT();
    if (res == BLS_FAIL)
    {
        printf("Failed to initialize\n");
        return res;
    }

    OCT_rand(&IKM,RNG,32);
    //IKM.len=32;
    //for (i=0;i<IKM.len;i++)
    //        IKM.val[i]=i+1;

    res = BLS_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        printf("Failed to generate keys\n");
        return res;
    }

    printf("Private key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);

    BLS_CORE_SIGN(&SIG, &M, &S);
    printf("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_CORE_VERIFY(&SIG, &M, &W);
    if (res == BLS_OK) printf("Signature is OK\n");
    else printf("Signature is *NOT* OK\n");
    return res;
}

int bls_BLS24479(csprng *RNG)
{
    using namespace BLS24479;

    int i,res;
    char s[BGS_BLS24479];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BLS24479+1], sig[8 * BFS_BLS24479 + 1];
#else
    char w[8 * BFS_BLS24479 + 1], sig[BFS_BLS24479 + 1];
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};

    OCT_jstring(&M,message);

    res = BLS_INIT();
    if (res == BLS_FAIL)
    {
        printf("Failed to initialize\n");
        return res;
    }

    OCT_rand(&IKM,RNG,48);
    //IKM.len=48;
    //for (i=0;i<IKM.len;i++)
    //        IKM.val[i]=i+1;

    res = BLS_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        printf("Failed to generate keys\n");
        return res;
    }

    printf("Private key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);

    BLS_CORE_SIGN(&SIG, &M, &S);
    printf("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_CORE_VERIFY(&SIG, &M, &W);
    if (res == BLS_OK) printf("Signature is OK\n");
    else printf("Signature is *NOT* OK\n");
    return res;
}

int bls_BLS48556(csprng *RNG)
{
    using namespace BLS48556;

    int i,res;
    char s[BGS_BLS48556];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BLS48556+1], sig[16 * BFS_BLS48556 + 1];
#else
    char w[16 * BFS_BLS48556 + 1], sig[BFS_BLS48556 + 1];
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};

    OCT_jstring(&M,message);

    res = BLS_INIT();
    if (res == BLS_FAIL)
    {
        printf("Failed to initialize\n");
        return res;
    }

    OCT_rand(&IKM,RNG,64);
    //IKM.len=64;
    //for (i=0;i<IKM.len;i++)
    //        IKM.val[i]=i+1;

    res = BLS_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        printf("Failed to generate keys\n");
        return res;
    }

    printf("Private key= 0x");
    OCT_output(&S);
    printf("Public key= 0x");
    OCT_output(&W);

    BLS_CORE_SIGN(&SIG, &M, &S);
    printf("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_CORE_VERIFY(&SIG, &M, &W);
    if (res == BLS_OK) printf("Signature is OK\n");
    else printf("Signature is *NOT* OK\n");
    return res;
}

#endif

int main()
{
    int i, res;
    unsigned long ran;

    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNG;                // Crypto Strong RNG

    time((time_t *)&ran);

    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (i = 4; i < 100; i++) RAW.val[i] = i;

    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG

    printf("%d bit build\n", CHUNK);

    printf("\nTesting BLS signature for curve BN254\n");
    bls_BN254(&RNG);

#if CHUNK!=16
    printf("\nTesting BLS signature for curve BLS383\n");
    bls_BLS12383(&RNG);

    printf("\nTesting BLS signature for curve BLS24479\n");
    bls_BLS24479(&RNG);

    printf("\nTesting BLS signature for curve BLS48556\n");
    bls_BLS48556(&RNG);
#endif

    KILL_CSPRNG(&RNG);
}


