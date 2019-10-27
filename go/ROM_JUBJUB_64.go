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

package JUBJUB

//*** rom field parameters*****
// Base Bits= 56
var Modulus= [...]Chunk {0xFFFFFF00000001,0xA402FFFE5BFEFF,0x809A1D80553BD,0x299D7D483339D8,0x73EDA753}
var R2modp= [...]Chunk {0x3B3440EC31BBA9,0x8929657E045FB0,0x2D645CF57C6E1A,0xEA6A1C5012ECF5,0x3C7B9D12}
var ROI= [...]Chunk {0x788F500B912F1F,0x4FF270B3E0941B,0xC8D168D6C0C402,0x5B416B6F0FD56D,0x212D79E}
const MConst Chunk=0xFFFFFEFFFFFFFF

//*** rom curve parameters *****
// Base Bits= 56

const CURVE_A int= -1
const CURVE_Cof_I int= 8
var CURVE_Cof= [...]Chunk {0x8,0x0,0x0,0x0,0x0}
const CURVE_B_I int= 0
var CURVE_B= [...]Chunk {0x65FD6D6343EB1,0x7F6D37579D2601,0x7E6BD7FD4292D,0x4BFA2B48F5FD92,0x2A9318E7}
var CURVE_Order= [...]Chunk {0x970E5ED6F72CB7,0x2093CCC81082D0,0x101343B00A668,0x6533AFA906673B,0xE7DB4EA}
var CURVE_Gx= [...]Chunk {0x4ECF1A74F976C4,0x546BF2F39EDE7F,0xDF00384882000C,0xF8EFF38CA624B4,0x5183972A}
var CURVE_Gy= [...]Chunk {0x6BAD709349702E,0x8707FFA6833B14,0x5ABD9DC308096C,0x2CA2FC2C9E8FCC,0x3B43F847}
