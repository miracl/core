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


package org.miracl.core.NUMS384E;

public class ROM
{
// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFEC3L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] R2modp= {0x188890000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x13DL;

	public static final int CURVE_Cof_I= 4;
	public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= -11556;
	public static final long[] CURVE_B= {0xFFFFFFFFFFD19FL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] CURVE_Order= {0xB9DCC4E6A3897DL,0x555AAB35C87920L,0x1CB46BE1CF61E4L,0xFFFFFFFFE2471AL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0xD540E46C206BDEL,0x92B16545941350L,0xA8F33163406FF2L,0xE5BE4C005763FFL,0xE55DB5B30BF446L,0x266CC0B6A2129AL,0x61B111FB45A9L};
	public static final long[] CURVE_Gy= {0x8D03E1F0729392L,0xB0F946EC48DC9DL,0xF7F645964B0072L,0xF1425F56830F98L,0xB10DD716AD8274L,0xEEB08738B1A423L,0x82983E67B9A6L};
}
