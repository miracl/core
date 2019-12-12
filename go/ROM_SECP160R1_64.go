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

package SECP160R1

// Base Bits= 56
var Modulus= [...]Chunk {0xFFFFFF7FFFFFFF,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF}
var R2modp= [...]Chunk {0x1000000010000,0x400000,0x0}
var ROI= [...]Chunk {0xFFFFFF7FFFFFFE,0xFFFFFFFFFFFFFF,0xFFFFFFFFFFFF}
const MConst Chunk=0xFFFFFF80000001

const CURVE_A int= -3
const CURVE_Cof_I int= 1
var CURVE_Cof= [...]Chunk {0x1,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0xD4D4ADC565FA45,0x7A8B65ACF89F81,0x1C97BEFC54BD}
var CURVE_Order= [...]Chunk {0x27AED3CA752257,0x1F4C8F9,0x1000000000000}
var CURVE_Gx= [...]Chunk {0xC38BB913CBFC82,0x73284664698968,0x4A96B5688EF5}
var CURVE_Gy= [...]Chunk {0x2351377AC5FB32,0x947D59DCC91204,0x23A628553168}
