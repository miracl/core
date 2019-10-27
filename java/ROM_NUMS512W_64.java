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


package org.miracl.core.NUMS512W;

public class ROM
{

// Base Bits= 60
	public static final long[] Modulus= {0xFFFFFFFFFFFFDC7L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFFDC6L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] R2modp= {0x100000000000000L,0x4F0BL,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x239L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 121243;
	public static final long[] CURVE_B= {0x1D99BL,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xE153F390433555DL,0x568B36607CD243CL,0x258ED97D0BDC63BL,0xA4FB94E7831B4FCL,0xFFFFFFFFFFF5B3CL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Gx= {0xC8287958CABAE57L,0x5D60137D6F5DE2DL,0x94286255615831DL,0xA151076B359E937L,0xC25306D9F95021L,0x3BB501F6854506EL,0x2A03D3B5298CAD8L,0x141D0A93DA2B700L,0x3AC03447L};
	public static final long[] CURVE_Gy= {0x3A08760383527A6L,0x2B5C1E4CFD0FE92L,0x1A840B25A5602CFL,0x15DA8B0EEDE9C12L,0x60C7BD14F14A284L,0xDEABBCBB8C8F4B2L,0xC63EBB1004B97DBL,0x29AD56B3CE0EEEDL,0x943A54CAL};
}
