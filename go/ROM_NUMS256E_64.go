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

package NUMS256E

// Base Bits= 56
var Modulus = [...]Chunk{0xFFFFFFFFFFFF43, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var ROI = [...]Chunk{0xFFFFFFFFFFFF42, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var R2modp = [...]Chunk{0x89000000000000, 0x8B, 0x0, 0x0, 0x0}

const MConst Chunk = 0xBD

const CURVE_Cof_I int = 4

var CURVE_Cof = [...]Chunk{0x4, 0x0, 0x0, 0x0, 0x0}

const CURVE_A int = 1
const CURVE_B_I int = -15342

var CURVE_B = [...]Chunk{0xFFFFFFFFFFC355, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFFFFFFFF, 0xFFFFFFFF}
var CURVE_Order = [...]Chunk{0x47B190EEDD4AF5, 0x5AA52F59439B1A, 0x4195, 0x0, 0x40000000}
var CURVE_Gx = [...]Chunk{0xDEC0902EED13DA, 0x8A0EE3083586A0, 0x5F69209BD60C39, 0x6AEA237DCD1E3D, 0x8A7514FB}
var CURVE_Gy = [...]Chunk{0xA616E7798A89E6, 0x61D810856ED32F, 0xD9A64B8010715F, 0xD9D925C7CE9665, 0x44D53E9F}
