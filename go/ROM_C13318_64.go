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

package C13318

//*** rom field parameters*****
// Base Bits= 56
var Modulus= [...]Chunk {0xFFFFFFFFFFFFED,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFFFF,0x7FFFFFFF}
var R2modp= [...]Chunk {0xA4000000000000,0x5,0x0,0x0,0x0}
var ROI= [...]Chunk {0xEE1B274A0EA0B0,0x1806AD2FE478C4,0x993DFBD7A72F43,0x4FC1DF0B2B4D00,0x2B832480}
const MConst Chunk=0x13

//*** rom curve parameters *****
// Base Bits= 56

const CURVE_A int= -3
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 13318
var CURVE_B= [...]Chunk {0x3406,0x0,0x0,0x0,0x0}
var CURVE_Order= [...]Chunk {0x7D12DC4DC2CBE3,0x54F83DEB8D16EB,0xF4F6,0x0,0x80000000}
var CURVE_Gx= [...]Chunk {0x5,0x0,0x0,0x0,0x0}
var CURVE_Gy= [...]Chunk {0xC8D3546B6EAD0B,0xCB04CF016DB5B8,0xE5166966369D4C,0x26BCA6F1381630,0x6675AAD9}
