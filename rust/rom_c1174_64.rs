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
use crate::c1174::big::NLEN;

// Base Bits= 56
// c1174 Modulus
pub const MODULUS:[Chunk;NLEN]=[0xFFFFFFFFFFFFF7,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const R2MODP:[Chunk;NLEN]=[0x0,0x144,0x0,0x0,0x0];
pub const ROI:[Chunk;NLEN]=[0xFFFFFFFFFFFFF6,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const MCONST:Chunk=0x9;

//*** rom curve parameters *****
// Base Bits= 56

pub const CURVE_A:isize = 1;
pub const CURVE_COF_I:isize = 4;
pub const CURVE_COF:[Chunk;NLEN]=[0x4,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = -1174;
pub const CURVE_B:[Chunk;NLEN]=[0xFFFFFFFFFFFB61,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFF];
pub const CURVE_ORDER:[Chunk;NLEN]=[0x44D45FD166C971,0x65C4DFD3073489,0xFFFFFFFFFFF779,0xFFFFFFFFFFFFFF,0x1FFFFFF];
pub const CURVE_GX:[Chunk;NLEN]=[0x123F27BCE29EDA,0xD96A492ECD6516,0xE7C029A190C021,0xEA308C479343AE,0x37FBB0C];
pub const CURVE_GY:[Chunk;NLEN]=[0xCCB1BF9B46360E,0xDEE2AF3F976BA4,0x1169840E0C4FE2,0xD47FB7CC665684,0x6B72F82];
