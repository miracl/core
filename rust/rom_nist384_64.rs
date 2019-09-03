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
use crate::nist384::big::NLEN;

// Base Bits= 56
// nist384 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFF,
    0xFFFF0000000000,
    0xFFFFFFFFFEFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [
    0xFE000000010000,
    0xFFFFFF,
    0x2,
    0xFFFFFFFE00,
    0x1000000020000,
    0x0,
    0x0,
];
pub const MCONST: Chunk = 0x100000001;

// nist384 Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0x85C8EDD3EC2AEF,
    0x398D8A2ED19D2A,
    0x8F5013875AC656,
    0xFE814112031408,
    0xF82D19181D9C6E,
    0xE7E4988E056BE3,
    0xB3312FA7E23E,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xEC196ACCC52973,
    0xDB248B0A77AEC,
    0x81F4372DDF581A,
    0xFFFFFFFFC7634D,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x545E3872760AB7,
    0xF25DBF55296C3A,
    0xE082542A385502,
    0x8BA79B9859F741,
    0x20AD746E1D3B62,
    0x5378EB1C71EF3,
    0xAA87CA22BE8B,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x431D7C90EA0E5F,
    0xB1CE1D7E819D7A,
    0x13B5F0B8C00A60,
    0x289A147CE9DA31,
    0x92DC29F8F41DBD,
    0x2C6F5D9E98BF92,
    0x3617DE4A9626,
];
