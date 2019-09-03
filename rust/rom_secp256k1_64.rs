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

// Base Bits= 56
// secp256k1 modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFEFFFFFC2F,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0xA1000000000000, 0x7A2000E90, 0x1, 0x0, 0x0];
pub const MCONST: Chunk = 0x38091DD2253531;

// secp256k1 curve
pub const CURVE_A: isize = 0;
pub const CURVE_COF_I: isize = 1;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B_I: isize = 7;
pub const CURVE_B: [Chunk; NLEN] = [0x7, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xD25E8CD0364141,
    0xDCE6AF48A03BBF,
    0xFFFFFFFFFEBAAE,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xF2815B16F81798,
    0xFCDB2DCE28D959,
    0x95CE870B07029B,
    0xF9DCBBAC55A062,
    0x79BE667E,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x47D08FFB10D4B8,
    0xB448A68554199C,
    0xFC0E1108A8FD17,
    0x26A3C4655DA4FB,
    0x483ADA77,
];
