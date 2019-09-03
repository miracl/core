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
#include "newhope.h"

using namespace core;

int main()
{
    int i, ii, j, err;
    unsigned long ran;
    char raw[100];
    csprng SRNG, CRNG;
    RAND_clean(&SRNG); RAND_clean(&CRNG);
    char s[1792], sb[1824], uc[2176], keyA[32], keyB[32];

    octet S = {0, sizeof(s), s};
    octet SB = {0, sizeof(sb), sb};
    octet UC = {0, sizeof(uc), uc};
    octet KEYA = {0, sizeof(keyA), keyA};
    octet KEYB = {0, sizeof(keyB), keyB};

    printf("\nTesting New Hope Key Exchange\n");

// initialise random number generators
    time((time_t *)&ran);

    raw[0] = ran;
    raw[1] = ran >> 8;
    raw[2] = ran >> 16;
    raw[3] = ran >> 24;

    for (i = 4; i < 100; i++) raw[i] = i + 1;
    RAND_seed(&SRNG, 100, raw);
    for (i = 4; i < 100; i++) raw[i] = i + 2;
    RAND_seed(&CRNG, 100, raw);
// NewHope Simple key exchange

    NHS_SERVER_1(&SRNG, &SB, &S);
    NHS_CLIENT(&CRNG, &SB, &UC, &KEYB);
    NHS_SERVER_2(&S, &UC, &KEYA);

    printf("Alice key= 0x");
    OCT_output(&KEYA);
    printf("Bob's key= 0x");
    OCT_output(&KEYB);

    return 0;
}


