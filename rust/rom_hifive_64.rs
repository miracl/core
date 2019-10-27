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
use crate::hifive::big::NLEN;

// Base Bits= 60
// hifive Modulus
pub const MODULUS: [Chunk; NLEN] = [
    0xFFFFFFFFFFFFFFD,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFFFFFFFF,
    0xFFFFFFFFF,
];
pub const ROI:[Chunk;NLEN]=[0xB804827F559D3B8,0xC94538B93EAC636,0x1EDA9FCA8B95DA4,0xB4553DF798C41F6,0x1EF527811FC38B2,0xF93FC3E03];
pub const R2MODP: [Chunk; NLEN] = [0x9000000000000, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const MCONST: Chunk = 0x3;

// hifive Curve
pub const CURVE_COF_I: isize = 8;
pub const CURVE_A: isize = 1;
pub const CURVE_B_I: isize = 11111;
pub const CURVE_COF: [Chunk; NLEN] = [0x8, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_B: [Chunk; NLEN] = [0x2B67, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_ORDER: [Chunk; NLEN] = [
    0xB2F95973E9FA805,
    0xC0BD6B87F93BAA7,
    0x71415FA9850,
    0x0,
    0x0,
    0x200000000,
];
pub const CURVE_GX: [Chunk; NLEN] = [0xC, 0x0, 0x0, 0x0, 0x0, 0x0];
pub const CURVE_GY: [Chunk; NLEN] = [
    0x2BEC68505FE8632,
    0x5D5650CA0365DB1,
    0x3811C7EF435B6DB,
    0x7853D1B14B46C,
    0x56502E18E1C161D,
    0xC0DC616B,
];
