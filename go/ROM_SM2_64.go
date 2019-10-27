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

// Base Bits= 56
var Modulus= [...]Chunk {0xFFFFFFFFFFFFFF,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE}
var ROI= [...]Chunk {0xFFFFFFFFFFFFFE,0xFFFF00000000FF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFE}
var R2modp= [...]Chunk {0x3000000040000,0xFFFFFFFE000000,0x300000002,0x10000000100,0x60000}
const MConst Chunk=0x1

//*** rom curve parameters *****
// Base Bits= 56

const CURVE_A int= -3
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0xBCBD414D940E93,0x89F515AB8F92DD,0x4BCF6509A7F397,0x9D9F5E344D5A9E,0x28E9FA9E}
var CURVE_Order= [...]Chunk {0xBBF40939D54123,0xDF6B21C6052B53,0xFFFFFFFFFF7203,0xFFFFFFFFFFFFFF,0xFFFFFFFE}
var CURVE_Gx= [...]Chunk {0x5A4589334C74C7,0xBBFF2660BE171,0x466A39C9948FE3,0x1F1981195F9904,0x32C4AE2C}
var CURVE_Gy= [...]Chunk {0xDF32E52139F0A0,0x877CC62A474002,0xE36B692153D0A9,0xF4F6779C59BDCE,0xBC3736A2}
