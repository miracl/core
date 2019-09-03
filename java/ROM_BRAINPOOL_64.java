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

/* Note that the original curve has been transformed to an isomorphic curve with A=-3 */

package org.miracl.core.BRAINPOOL;

public class ROM
{

// Base Bits= 56
	public static final long[] Modulus= {0x13481D1F6E5377L,0xF623D526202820L,0x909D838D726E3BL,0xA1EEA9BC3E660AL,0xA9FB57DBL};
	public static final long[] R2modp= {0x9E04F49B9A3787L,0x29317218F3CF49L,0x54E8C3CF1DBC89L,0xBB411A3F7559CAL,0x9773E15FL};
	public static final long MConst= 0xA75590CEFD89B9L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 0;
	public static final long[] CURVE_B= {0xE58101FEE92B04L,0xEBC4AF2F49256AL,0x733D0B76B7BF93L,0x30D84EA4FE66A7L,0x662C61C4L};
	public static final long[] CURVE_Order= {0x1E0E82974856A7L,0x7AA3B561A6F790L,0x909D838D718C39L,0xA1EEA9BC3E660AL,0xA9FB57DBL};
	public static final long[] CURVE_Gx= {0xA191562E1305F4L,0x42C47AAFBC2B79L,0xB23A656149AFA1L,0xC1CFE7B7732213L,0xA3E8EB3CL};
	public static final long[] CURVE_Gy= {0xABE8F35B25C9BEL,0xB6DE39D027001DL,0xE14644417E69BCL,0x3439C56D7F7B22L,0x2D996C82L};
}

