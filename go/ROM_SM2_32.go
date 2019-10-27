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

package SM2

// Base Bits= 28
var Modulus= [...]Chunk {0xFFFFFFF,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var ROI= [...]Chunk {0xFFFFFFE,0xFFFFFFF,0xFF,0xFFFF000,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var R2modp= [...]Chunk {0x40000,0x300000,0xE000000,0xFFFFFFF,0x2,0x30,0x100,0x1000,0x60000,0x0}
const MConst Chunk=0x1

//*** rom curve parameters *****
// Base Bits= 28

const CURVE_A int= -3
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0xD940E93,0xBCBD414,0xB8F92DD,0x89F515A,0x9A7F397,0x4BCF650,0x44D5A9E,0x9D9F5E3,0x8E9FA9E,0x2}
var CURVE_Order= [...]Chunk {0x9D54123,0xBBF4093,0x6052B53,0xDF6B21C,0xFFF7203,0xFFFFFFF,0xFFFFFFF,0xFFFFFFF,0xFFFFFFE,0xF}
var CURVE_Gx= [...]Chunk {0x34C74C7,0x5A45893,0x60BE171,0xBBFF26,0x9948FE3,0x466A39C,0x95F9904,0x1F19811,0x2C4AE2C,0x3}
var CURVE_Gy= [...]Chunk {0x139F0A0,0xDF32E52,0xA474002,0x877CC62,0x153D0A9,0xE36B692,0xC59BDCE,0xF4F6779,0xC3736A2,0xB}
