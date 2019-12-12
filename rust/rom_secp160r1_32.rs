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
use crate::secp160r1::big::NLEN;

// Base Bits= 29
pub const MODULUS:[Chunk;NLEN]=[0x1FFFFFFF,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF];
pub const R2MODP:[Chunk;NLEN]=[0x10000000,0x0,0x4,0x8,0x0,0x0];
pub const ROI:[Chunk;NLEN]=[0x1FFFFFFE,0x1FFFFFFB,0x1FFFFFFF,0x1FFFFFFF,0x1FFFFFFF,0x7FFF];
pub const MCONST:Chunk=0x1;

pub const CURVE_A:isize = -3;
pub const CURVE_COF_I:isize = 1;
pub const CURVE_COF:[Chunk;NLEN]=[0x1,0x0,0x0,0x0,0x0,0x0];
pub const CURVE_B_I:isize = 0;
pub const CURVE_B:[Chunk;NLEN]=[0x565FA45,0xEA6A56E,0xB3E27E0,0x1AF516CB,0x1BEFC54B,0xE4B];
pub const CURVE_ORDER:[Chunk;NLEN]=[0xA752257,0x93D769E,0x7D323E,0x0,0x0,0x8000];
pub const CURVE_GX:[Chunk;NLEN]=[0x13CBFC82,0x61C5DC8,0x191A625A,0xAE6508C,0xB5688EF,0x254B];
pub const CURVE_GY:[Chunk;NLEN]=[0x1AC5FB32,0x11A89BB,0x17324481,0x1128FAB3,0x2855316,0x11D3];
