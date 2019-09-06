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
#include <bls_XXX.h>
#include <randapi.h>

#define HAVE_ECCX08

#ifdef HAVE_ECCX08
#include <ArduinoECCX08.h>
#endif

using namespace core;

csprng RNG;                // Crypto Strong RNG

static char message[] = "This is a test message";

int bls_XXX()
{
    using namespace XXX;

    int res;
    char s[BGS_XXX], w[4 * BFS_XXX + 1], sig[BFS_XXX + 1];
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};

    Serial.println("Generating key pair");
    res = BLS_KEY_PAIR_GENERATE(&RNG, &S, &W);
    if (res == BLS_FAIL)
    {
        Serial.println("Failed to generate keys");
        return 0;
    }

    Serial.print("Private key= 0x");
    OCT_output(&S);
    Serial.print("Public key= 0x");
    OCT_output(&W);

    Serial.println("Signing");
    BLS_SIGN(&SIG, message, &S);
    Serial.print("Signature= 0x");
    OCT_output(&SIG);

    //message[7]='f'; // change the message
    Serial.println("Verifying");
    res = BLS_VERIFY(&SIG, message, &W);
    if (res == BLS_OK)
    {
        Serial.println("Signature is OK");
        return 1;
    }
    else
    {
        Serial.println("Signature is *NOT* OK");
        return 0;
    }
}

void setup()
{
    int i;
    Serial.begin(115200);
    while (!Serial);

#ifdef HAVE_ECCX08
    if (!ECCX08.begin()) {
        Serial.println("Failed to communicate with ECC508/ECC608!");
        while (1);
    }
#endif
    char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    RAW.len = 100;
#ifdef HAVE_ECCX08
    for (i = 0; i < 100; i++) RAW.val[i] = ECCX08.random(256);
#else
    for (i = 0; i < 100; i++) RAW.val[i] = i + 1;
#endif
    CREATE_CSPRNG(&RNG, &RAW);  // initialise strong RNG
}

void loop()
{
    Serial.println("Testing BLS signature for curve XXX");
    if (!bls_XXX())
    {
        Serial.println("BLS failed");
        while (1) ;
    }
}


