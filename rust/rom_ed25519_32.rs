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

use crate::arch::Chunk;
use crate::ed25519::big::NLEN;

// Base Bits= 29
// Curve25519 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0x1FFFFFED, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF, 0x1FFFFFFF,
    0x7FFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x169000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x13;

// Ed25519 Curve
pub const CURVE_COF_I: isize = 8;
pub const CURVE_A: isize = -1;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0x135978A3, 0xF5A6E50, 0x10762ADD, 0x149A82, 0x1E898007, 0x3CBBBC, 0x19CE331D, 0x1DC56DFF,
    0x52036C,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0x1CF5D3ED, 0x9318D2, 0x1DE73596, 0x1DF3BD45, 0x14D, 0x0, 0x0, 0x0, 0x100000,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xF25D51A, 0xAB16B04, 0x969ECB2, 0x198EC12A, 0xDC5C692, 0x1118FEEB, 0xFFB0293, 0x1A79ADCA,
    0x216936,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x6666658, 0x13333333, 0x19999999, 0xCCCCCCC, 0x6666666, 0x13333333, 0x19999999, 0xCCCCCCC,
    0x666666,
];
