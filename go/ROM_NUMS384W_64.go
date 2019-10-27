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

package NUMS384W

// Base Bits= 58
var Modulus = [...]Chunk{0x3FFFFFFFFFFFEC3, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0xFFFFFFFFF}
var ROI = [...]Chunk{0x3FFFFFFFFFFFEC2, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0xFFFFFFFFF}
var R2modp = [...]Chunk{0x88900000000000, 0x6, 0x0, 0x0, 0x0, 0x0, 0x0}

const MConst Chunk = 0x13D

const CURVE_Cof_I int = 1

var CURVE_Cof = [...]Chunk{0x1, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = -3
const CURVE_B_I int = -34568

var CURVE_B = [...]Chunk{0x3FFFFFFFFFF77BB, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0xFFFFFFFFF}
var CURVE_Order = [...]Chunk{0x4D81F67B0E61B9, 0x2A74F530DF89E98, 0x2F1EEB5D6881BED, 0x3FFFFFFFFFF587A, 0x3FFFFFFFFFFFFFF, 0x3FFFFFFFFFFFFFF, 0xFFFFFFFFF}
var CURVE_Gx = [...]Chunk{0x39CBA042098152A, 0x3BB4403D8711EFA, 0x291B2A6CC742522, 0x337EE07E7D3CEF4, 0x24105F1A60225C1, 0x5BC60712203288, 0x757956F0B}
var CURVE_Gy = [...]Chunk{0x74B8EC66180716, 0x1AD36EFFD2B4997, 0x37D121A837EBCD, 0x1DFE1FDCE72E482, 0x584CF7EB00469, 0x66E38E35F8CF4C, 0xACDEE368E}
