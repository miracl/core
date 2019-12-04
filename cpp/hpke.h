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
 * @file hpke.h
 * @author Mike Scott 
 * @date 2nd December 2019
 * @brief HPKE Header file
 *
 * declares functions
 *
 */

#ifndef HPKE_ZZZ_H
#define HPKE_ZZZ_H

#include "ecdh_ZZZ.h"

using namespace core;

//#define CONFIG_ID 0x16 // 01|01|10 = 1, 1, 2
//#define KEM_ID 2  // Curve 25519
//#define KDF_ID 1  // HKDF-SHA256
//#define AEAD_ID 1 // AES-GCM-128

#define HPKE_OK                     0     /**< Function completed without error */
#define HPKE_INVALID_PUBLIC_KEY    -2	/**< Public Key is Invalid */
#define HPKE_ERROR                 -3	/**< HPKE Internal Error */

namespace ZZZ {

/* HPKE DHKEM primitives */
/**	@brief Encapsulate function
 *
    @param config_id is the configuration KEM/KDF/AEAD
	@param R is a pointer to a cryptographically secure random number generator
    @param SK is the input ephemeral secret if RNG==NULL, otherwise set to NULL
    @param Z is a pointer to a shared secret DH(skE,pkR)
	@param pkE the ephemeral public key, which is skE.G, where G is a fixed generator
	@param pkR the respondents public key
 */
extern void HPKE_Encap(int config_id,csprng *R,octet *SK,octet *Z,octet *pkE,octet *pkR);

/**	@brief Decapsulate function
 *
    @param config_id is the configuration KEM/KDF/AEAD
    @param Z is a pointer to a shared secret DH(skR,pkE)
	@param pkE the ephemeral public key
	@param skR the respondents private key
 */
extern void HPKE_Decap(int config_id,octet *Z,octet *pkE,octet *skR);

/**	@brief Encapsulate/Authenticate function
 *
    @param config_id is the configuration KEM/KDF/AEAD
	@param R is a pointer to a cryptographically secure random number generator
    @param SK is the input ephemeral secret if RNG==NULL, otherwise set to NULL
    @param Z is a pointer to a shared secret DH(skE,pkR)
	@param pkE the ephemeral public key, which is skE.G, where G is a fixed generator
	@param pkR the Respondents public key
    @param skI the Initiators private key
 */
extern void HPKE_AuthEncap(int config_id,csprng *R,octet *SK,octet *Z,octet *pkE,octet *pkR,octet *skI);

/**	@brief Decapsulate function
 *
    @param config_id is the configuration KEM/KDF/AEAD
    @param Z is a pointer to a shared secret DH(skR,pkE)
	@param pkE the ephemeral public key
	@param skR the Respondents private key
    @param pkI the Initiators public key
 */
extern void HPKE_AuthDecap(int config_id,octet *Z,octet *pkE,octet *skR,octet *pkI);

/**	@brief KeyScheduler function
 *
    @param config_id is the configuration KEM/KDF/AEAD
    @param key  the output key for aead encryption
    @param nonce  the output nonce for aead encryption
	@param mode  the mode of operation
	@param pkR the Respondents private key
    @param Z the shared key
    @param pkE the Ephemeral public key
    @param info application dependent info
    @param psk pre-shared key
    @param pskID identifier for the psk
    @param pkI Initiators public key
 */
extern void HPKE_KeySchedule(int config_id,octet *key,octet *nonce,int mode,octet *pkR,octet *Z,octet *pkE,octet *info,octet *psk,octet *pskID,octet *pkI);

}
#endif
