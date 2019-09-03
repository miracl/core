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

use crate::anssi::big::NLEN;
use crate::arch::Chunk;

// Base Bits= 56
// anssi Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFCF353D86E9C03,
    0xADBCABC8CA6DE8,
    0xE8CE42435B3961,
    0xB3AD58F10126D,
    0xF1FD178C,
];
pub const R2MODP: [Chunk; NLEN] = [
    0x18D2374288CC9C,
    0x4929E67646BD2B,
    0x220E6C1D6F7F2D,
    0x751B1FDABCE02E,
    0xE7401B78,
];
pub const MCONST: Chunk = 0x97483A164E1155;

// anssi Curve

pub const CURVE_COF_I: isize = 1;
pub const CURVE_A: isize = -3;
pub const CURVE_B_I: isize = 0;
pub const CURVE_COF: [Chunk; NLEN] = [0x1, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [
    0x75ED967B7BB73F,
    0xC9AE4B1A18030,
    0x754A44C00FDFEC,
    0x5428A9300D4ABA,
    0xEE353FCA,
];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xFDD459C6D655E1,
    0x67E140D2BF941F,
    0xE8CE42435B53DC,
    0xB3AD58F10126D,
    0xF1FD178C,
];
pub const CURVE_GX: [Chunk; NLEN] = [
    0xC97A2DD98F5CFF,
    0xD2DCAF98B70164,
    0x4749D423958C27,
    0x56C139EB31183D,
    0xB6B3D4C3,
];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x115A1554062CFB,
    0xC307E8E4C9E183,
    0xF0F3ECEF8C2701,
    0xC8B204911F9271,
    0x6142E0F7,
];
