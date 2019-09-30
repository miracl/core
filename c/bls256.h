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

/**
 * @file bls256.h
 * @author Mike Scott
 * @date 28th Novemebr 2018
 * @brief BLS Header file
 *
 * Allows some user configuration
 * defines structures
 * declares functions
 *
 */

#ifndef BLS_ZZZ_H
#define BLS_ZZZ_H

#include "pair256_ZZZ.h"

/* Field size is assumed to be greater than or equal to group size */

#define BGS_ZZZ MODBYTES_XXX  /**< BLS Group Size */
#define BFS_ZZZ MODBYTES_XXX  /**< BLS Field Size */

#define BLS_OK           0  /**< Function completed without error */
#define BLS_FAIL        -1  /**< Point is NOT on the curve */

/* BLS API functions */

/** @brief Initialise BLS
 *
    @return BLS_OK if worked, otherwise BLS_FAIL
 */
int BLS_ZZZ_INIT();

/** @brief Generate Key Pair
 *
    @param IKM is an octet containing random Initial Keying Material
    @param S on output a private key
    @param W on output a public key = S*G, where G is fixed generator
    @return BLS_OK
 */
int BLS_ZZZ_KEY_PAIR_GENERATE(octet *IKM, octet* S, octet *W);

/** @brief Calculate a signature
 *
    @param SIG the ouput signature
    @param M is the message to be signed
    @param S an input private key
    @return BLS_OK
 */
int BLS_ZZZ_CORE_SIGN(octet *SIG, octet *M, octet *S);

/** @brief Verify a signature
 *
    @param SIG an input signature
    @param M is the message whose signature is to be verified.
    @param W an public key
    @return BLS_OK if verified, otherwise BLS_FAIL
 */
int BLS_ZZZ_CORE_VERIFY(octet *SIG, octet *M, octet *W);

#endif

