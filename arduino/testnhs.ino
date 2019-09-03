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

/* test driver and function exerciser for NewHope Functions */

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

/* gcc -O2 testnhs.c core.a -o testnhs.exe */

#include <stdio.h>
#include <time.h>
#include <newhope.h>
#include <randapi.h>

#define HAVE_ECCX08

#ifdef HAVE_ECCX08
#include <ArduinoECCX08.h>
#endif

using namespace core;

csprng SRNG, CRNG;

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
    CREATE_CSPRNG(&SRNG, &RAW);  // initialise strong RNG

#ifdef HAVE_ECCX08
    for (i = 0; i < 100; i++) RAW.val[i] = ECCX08.random(256);
#else
    for (i = 0; i < 100; i++) RAW.val[i] = i + 1;
#endif
    CREATE_CSPRNG(&CRNG, &RAW);  // initialise strong RNG

}

void loop()
{
    int i, ii, j, err;
    char s[1792], sb[1824], uc[2176], keyA[32], keyB[32];

    octet S = {0, sizeof(s), s};
    octet SB = {0, sizeof(sb), sb};
    octet UC = {0, sizeof(uc), uc};
    octet KEYA = {0, sizeof(keyA), keyA};
    octet KEYB = {0, sizeof(keyB), keyB};

// NewHope Simple key exchange

    NHS_SERVER_1(&SRNG, &SB, &S);
    NHS_CLIENT(&CRNG, &SB, &UC, &KEYB);
    NHS_SERVER_2(&S, &UC, &KEYA);

    Serial.print("Alice key= 0x");
    OCT_output(&KEYA);
    Serial.print("Bob's key= 0x");
    OCT_output(&KEYB);
}


