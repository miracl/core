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


package org.miracl.core.NUMS384W;

public class ROM
{

// Base Bits= 58
	public static final long[] Modulus= {0x3FFFFFFFFFFFEC3L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] ROI= {0x3FFFFFFFFFFFEC2L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] R2modp= {0x88900000000000L,0x6L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x13DL;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= -34568;
	public static final long[] CURVE_B= {0x3FFFFFFFFFF77BBL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] CURVE_Order= {0x4D81F67B0E61B9L,0x2A74F530DF89E98L,0x2F1EEB5D6881BEDL,0x3FFFFFFFFFF587AL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x39CBA042098152AL,0x3BB4403D8711EFAL,0x291B2A6CC742522L,0x337EE07E7D3CEF4L,0x24105F1A60225C1L,0x5BC60712203288L,0x757956F0BL};
	public static final long[] CURVE_Gy= {0x74B8EC66180716L,0x1AD36EFFD2B4997L,0x37D121A837EBCDL,0x1DFE1FDCE72E482L,0x584CF7EB00469L,0x66E38E35F8CF4CL,0xACDEE368EL};


}
