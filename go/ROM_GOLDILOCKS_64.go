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

package GOLDILOCKS

// Base Bits= 58
var Modulus = [...]Chunk{0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FBFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFF}
var ROI = [...]Chunk{0x3FFFFFFFFFFFFFE, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FBFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFF}
var R2modp = [...]Chunk{0x200000000, 0x0, 0x0, 0x0, 0x3000000, 0x0, 0x0, 0x0}

const MConst Chunk = 0x1

const CURVE_Cof_I int = 4

var CURVE_Cof = [...]Chunk{0x4, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = 1
const CURVE_B_I int = -39081

var CURVE_B = [...]Chunk{0x3FFFFFFFFFF6756, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FBFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFF}
var CURVE_Order = [...]Chunk{0x378C292AB5844F3, 0x3309CA37163D548, 0x1B49AED63690216, 0x3FDF3288FA7113B, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0xFFFFFFFFFF}
var CURVE_Gx = [...]Chunk{0x155555555555555, 0x155555555555555, 0x155555555555555, 0x2A5555555555555, 0x2AAAAAAAAAAAAAA, 0x2AAAAAAAAAAAAAA, 0x2AAAAAAAAAAAAAA, 0x2AAAAAAAAAA}
var CURVE_Gy = [...]Chunk{0x2EAFBCDEA9386ED, 0x32CAFB473681AF6, 0x25833A2A3098BBB, 0x1CA2B6312E03595, 0x35884DD7B7E36D, 0x21B0AC00DBB5E8, 0x17048DB359D6205, 0x2B817A58D2B}
