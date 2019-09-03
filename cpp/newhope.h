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

/* NewHope Simple API */

#ifndef NHS_H
#define NHS_H

#include "core.h"

namespace core {
/** @brief NHS server first pass
 *
    @param RNG Random Number Generator handle
    @param SB seed and polynomial B concatenated - output
    @param S server secret - output

 */
extern void NHS_SERVER_1(csprng *RNG, octet *SB, octet *S);
/** @brief NHS client pass
 *
    @param RNG Random Number Generator handle
    @param SB seed and polynomial B concatenated - input
    @param UC polynomial U and compressed polynomial c - output
    @param KEY client key
 */
extern void NHS_CLIENT(csprng *RNG, octet *SB, octet *UC, octet *KEY);
/** @brief NHS server second pass
 *
    @param S server secret - input
    @param UC polynomial U and compressed polynomial c - input
    @param KEY server key
 */
extern void NHS_SERVER_2(octet *S, octet *UC, octet *KEY);

}

#endif
