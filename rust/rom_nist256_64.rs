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

use crate::arch::Chunk;
use crate::nist256::big::NLEN;

// Base Bits= 56
// nist256 modulus
pub const MODULUS: [Chunk; NLEN] = [0xFFFFFFFFFFFFFF, 0xFFFFFFFFFF, 0x0, 0x1000000, 0xFFFFFFFF];
pub const R2MODP: [Chunk; NLEN] = [
    0x3000000050000,
    0x0,
    0xFFFFFBFFFFFFFA,
    0xFFFAFFFFFFFEFF,
    0x2FFFF,
];
pub const MCONST: Chunk = 0x1;

// nist256 curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0xCE3C3E27D2604B,
    0x6B0CC53B0F63B,
    0x55769886BC651D,
    0xAA3A93E7B3EBBD,
    0x5AC635D8,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xB9CAC2FC632551,
    0xFAADA7179E84F3,
    0xFFFFFFFFFFBCE6,
    0xFFFFFF,
    0xFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xA13945D898C296,
    0x7D812DEB33A0F4,
    0xE563A440F27703,
    0xE12C4247F8BCE6,
    0x6B17D1F2,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0xB6406837BF51F5,
    0x33576B315ECECB,
    0x4A7C0F9E162BCE,
    0xFE1A7F9B8EE7EB,
    0x4FE342E2,
];
