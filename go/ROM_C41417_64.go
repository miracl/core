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

package C41417

// Base Bits= 60
var Modulus = [...]Chunk{0xFFFFFFFFFFFFFEF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFF}
var ROI = [...]Chunk{0xFFFFFFFFFFFFFEE, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFF}
var R2modp = [...]Chunk{0x121000, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0x11

const CURVE_Cof_I int = 8

var CURVE_Cof = [...]Chunk{0x8, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = 1
const CURVE_B_I int = 3617

var CURVE_B = [...]Chunk{0xE21, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
var CURVE_Order = [...]Chunk{0xB0E71A5E106AF79, 0x1C0338AD63CF181, 0x414CF706022B36F, 0xFFFFFFFFEB3CC92, 0xFFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFFF, 0x7FFFFFFFFFFFF}
var CURVE_Gx = [...]Chunk{0x4FD3812F3CBC595, 0x1A73FAA8537C64C, 0x4AB4D6D6BA11130, 0x3EC7F57FF35498A, 0xE5FCD46369F44C0, 0x300218C0631C326, 0x1A334905141443}
var CURVE_Gy = [...]Chunk{0x22, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}
