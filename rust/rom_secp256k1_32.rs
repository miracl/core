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
use crate::secp256k1::big::NLEN;

// Base Bits= 28
// secp256k1 modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFC2F, 0xFFFFFEF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFC2E, 0xFFFFFEF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x0, 0xA100000, 0x2000E90, 0x7A, 0x1, 0x0, 0x0, 0x0, 0x0, 0x0,
];
pub const MCONST: Chunk = 0x2253531;

// secp256k1 curve
pub const CURVE_A: isize = 0;
pub const CURVE_COF_I: isize = 1;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B_I: isize = 7;
pub const CURVE_B: [Chunk; NLEN] = [0x7, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x364141, 0xD25E8CD, 0x8A03BBF, 0xDCE6AF4, 0xFFEBAAE, 0xFFFFFFF, 0xFFFFFFF, 0xFFFFFFF,
    0xFFFFFFF, 0xF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x6F81798, 0xF2815B1, 0xE28D959, 0xFCDB2DC, 0xB07029B, 0x95CE870, 0xC55A062, 0xF9DCBBA,
    0x9BE667E, 0x7,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0xB10D4B8, 0x47D08FF, 0x554199C, 0xB448A68, 0x8A8FD17, 0xFC0E110, 0x55DA4FB, 0x26A3C46,
    0x83ADA77, 0x4,
];
