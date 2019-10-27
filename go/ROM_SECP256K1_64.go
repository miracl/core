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

package SECP256K1

// Base Bits= 56

var Modulus = [...]Chunk{0xFFFFFEFFFFFC2F, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var ROI = [...]Chunk{0xFFFFFEFFFFFC2E, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var R2modp = [...]Chunk{0xA1000000000000, 0x7A2000E90, 0x1, 0x0, 0x0}

const MConst Chunk = 0x38091DD2253531

// Base Bits= 56

const CURVE_A int = 0
const CURVE_Cof_I int = 1

var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0}

const CURVE_B_I int = 7

var CURVE_B = [...]Chunk{0x7, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0xD25E8CD0364141, 0xDCE6AF48A03BBF, 0xFFFFFFFFFEBAAE, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var CURVE_Gx = [...]Chunk{0xF2815B16F81798, 0xFCDB2DCE28D959, 0x95CE870B07029B, 0xF9DCBBAC55A062, 0x79BE667E}
var CURVE_Gy = [...]Chunk{0x47D08FFB10D4B8, 0xB448A68554199C, 0xFC0E1108A8FD17, 0x26A3C4655DA4FB, 0x483ADA77}
