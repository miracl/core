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

package NIST521

// Base Bits= 28
var Modulus = [...]Chunk{0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0x1FFFF}
var R2modp = [...]Chunk{0x400000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0x1

const CURVE_Cof_I int = 1

var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = -3
const CURVE_B_I int = 0

var CURVE_B = [...]Chunk{0xB503F00, 0x451FD46, 0xC34F1EF, 0xDF883D2, 0xF073573, 0xBD3BB1B, 0xB1652C0, 0xEC7E937, 0x6193951, 0xF109E15, 0x489918E, 0x15F3B8B, 0x25B99B3, 0xEEA2DA7, 0xB68540, 0x929A21A, 0xE1C9A1F, 0x3EB9618, 0x5195}
var CURVE_Order = [...]Chunk{0x1386409, 0x6FB71E9, 0xC47AEBB, 0xC9B8899, 0x5D03BB5, 0x48F709A, 0xB7FCC01, 0xBF2F966, 0x1868783, 0xFFFFFA5, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0x1FFFF}
var CURVE_Gx = [...]Chunk{0x2E5BD66, 0x7E7E31C, 0xA429BF9, 0xB3C1856, 0x8DE3348, 0x27A2FFA, 0x8FE1DC1, 0xEFE7592, 0x14B5E77, 0x4D3DBAA, 0x8AF606B, 0xB521F82, 0x139053F, 0x429C648, 0x62395B4, 0x9E3ECB6, 0x404E9CD, 0x8E06B70, 0xC685}
var CURVE_Gy = [...]Chunk{0xFD16650, 0xBE94769, 0x2C24088, 0x7086A27, 0x761353C, 0x13FAD0, 0xC550B9, 0x5EF4264, 0x7EE7299, 0x3E662C9, 0xFBD1727, 0x446817A, 0x449579B, 0xD998F54, 0x42C7D1B, 0x5C8A5FB, 0xA3BC004, 0x296A789, 0x11839}
