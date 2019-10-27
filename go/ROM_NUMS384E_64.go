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

/* Fixed Data in ROM - Field and Curve parameters */

package NUMS384E

// Base Bits= 56
var Modulus = [...]Chunk{0xFFFFFFFFFFFEC3, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF}
var ROI = [...]Chunk{0xFFFFFFFFFFFEC2, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF}
var R2modp = [...]Chunk{0x188890000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0x13D

const CURVE_Cof_I int = 4

var CURVE_Cof = [...]Chunk{0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = 1
const CURVE_B_I int = -11556

var CURVE_B = [...]Chunk{0xFFFFFFFFFFD19F, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFF}
var CURVE_Order = [...]Chunk{0xB9DCC4E6A3897D, 0x555AAB35C87920, 0x1CB46BE1CF61E4, 0xFFFFFFFFE2471A, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0x3FFFFFFFFFFF}
var CURVE_Gx = [...]Chunk{0xD540E46C206BDE, 0x92B16545941350, 0xA8F33163406FF2, 0xE5BE4C005763FF, 0xE55DB5B30BF446, 0x266CC0B6A2129A, 0x61B111FB45A9}
var CURVE_Gy = [...]Chunk{0x8D03E1F0729392, 0xB0F946EC48DC9D, 0xF7F645964B0072, 0xF1425F56830F98, 0xB10DD716AD8274, 0xEEB08738B1A423, 0x82983E67B9A6}
