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
use crate::nums256w::big::NLEN;

// Base Bits= 56
// nums256 Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFF43,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const R2MODP: [Chunk; NLEN] = [0x89000000000000, 0x8B, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0xBD;

// nums256w Curve
pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 152961;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x25581, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xAB20294751A825,
    0x8275EA265C6020,
    0xFFFFFFFFFFE43C,
    0xFFFFFFFFFFFFFF,
    0xFFFFFFFF,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0x52EE1EB21AACB1,
    0x9B0903D4C73ABC,
    0xA04F42CB098357,
    0x5AAADB61297A95,
    0xBC9ED6B6,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0xB5B9CB2184DE9F,
    0xC3D115310FBB80,
    0xF77E04E035C955,
    0x3399B6A673448B,
    0xD08FC0F1,
];
