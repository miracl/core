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

package C1665

// Base Bits= 60
var Modulus= [...]Chunk {0xFFFFFFFFFFFFFFB,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF}
var R2modp= [...]Chunk {0x190000000,0x0,0x0}
var ROI= [...]Chunk {0xFFFFFFFFFFFFFFA,0xFFFFFFFFFFFFFFF,0x3FFFFFFFFFFF}
const MConst Chunk=0x5

//*** rom curve parameters *****
// Base Bits= 60

const CURVE_A int= 1
const CURVE_Cof_I int= 4
var CURVE_Cof= [...]Chunk {0x4,0x0,0x0}
const CURVE_B_I int= 5766
var CURVE_B= [...]Chunk {0x1686,0x0,0x0}
var CURVE_Order= [...]Chunk {0x80FF0A99DBA8B27,0xFFFFFFFFFD5EF01,0xFFFFFFFFFFF}
var CURVE_Gx= [...]Chunk {0x671B9DBF9D52398,0x9A0618EE0F666C,0x14C94DA505B8}
var CURVE_Gy= [...]Chunk {0xC7087B244920345,0x13BFC7739D62DC2,0x29414549BC0A}