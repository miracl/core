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

// Base Bits= 28
// nist256 modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFF, 0x0, 0x0, 0x1000000, 0x0, 0xFFFFFFF, 0xF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFFFE, 0xFFFFFFF, 0xFFFFFFF, 0xFFF, 0x0, 0x0, 0x1000000, 0x0, 0xFFFFFFF, 0xF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x50000, 0x300000, 0x0, 0x0, 0xFFFFFFA, 0xFFFFFBF, 0xFFFFEFF, 0xFFFAFFF, 0x2FFFF, 0x0,
];
pub const MCONST: Chunk = 0x1;

// nist256 curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0x7D2604B, 0xCE3C3E2, 0x3B0F63B, 0x6B0CC5, 0x6BC651D, 0x5576988, 0x7B3EBBD, 0xAA3A93E,
    0xAC635D8, 0x5,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xC632551, 0xB9CAC2F, 0x79E84F3, 0xFAADA71, 0xFFFBCE6, 0xFFFFFFF, 0xFFFFFF, 0x0, 0xFFFFFFF,
    0xF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x898C296, 0xA13945D, 0xB33A0F4, 0x7D812DE, 0xF27703, 0xE563A44, 0x7F8BCE6, 0xE12C424,
    0xB17D1F2, 0x6,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x7BF51F5, 0xB640683, 0x15ECECB, 0x33576B3, 0xE162BCE, 0x4A7C0F9, 0xB8EE7EB, 0xFE1A7F9,
    0xFE342E2, 0x4,
];
