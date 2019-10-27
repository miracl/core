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
use crate::nist521::big::NLEN;

// Base Bits= 60
// nist521 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0x1FFFFFFFFFF,
];
pub const ROI: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFFFE,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0x1FFFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x4000000000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x1;

// nist521 Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0xF451FD46B503F00,
    0x73DF883D2C34F1E,
    0x2C0BD3BB1BF0735,
    0x3951EC7E937B165,
    0x9918EF109E15619,
    0x5B99B315F3B8B48,
    0xB68540EEA2DA72,
    0x8E1C9A1F929A21A,
    0x51953EB961,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xB6FB71E91386409,
    0xB5C9B8899C47AEB,
    0xC0148F709A5D03B,
    0x8783BF2F966B7FC,
    0xFFFFFFFFFFA5186,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0x1FFFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x97E7E31C2E5BD66,
    0x48B3C1856A429BF,
    0xDC127A2FFA8DE33,
    0x5E77EFE75928FE1,
    0xF606B4D3DBAA14B,
    0x39053FB521F828A,
    0x62395B4429C6481,
    0x404E9CD9E3ECB6,
    0xC6858E06B7,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x8BE94769FD16650,
    0x3C7086A272C2408,
    0xB9013FAD076135,
    0x72995EF42640C55,
    0xD17273E662C97EE,
    0x49579B446817AFB,
    0x42C7D1BD998F544,
    0x9A3BC0045C8A5FB,
    0x11839296A78,
];
