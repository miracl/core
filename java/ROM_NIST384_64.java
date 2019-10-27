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


package org.miracl.core.NIST384;

public class ROM
{

// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFL,0xFFFF0000000000L,0xFFFFFFFFFEFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFEL,0xFFFF0000000000L,0xFFFFFFFFFEFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] R2modp= {0xFE000000010000L,0xFFFFFFL,0x2L,0xFFFFFFFE00L,0x1000000020000L,0x0L,0x0L};
	public static final long MConst= 0x100000001L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 0;
	public static final long[] CURVE_B= {0x85C8EDD3EC2AEFL,0x398D8A2ED19D2AL,0x8F5013875AC656L,0xFE814112031408L,0xF82D19181D9C6EL,0xE7E4988E056BE3L,0xB3312FA7E23EL};
	public static final long[] CURVE_Order= {0xEC196ACCC52973L,0xDB248B0A77AECL,0x81F4372DDF581AL,0xFFFFFFFFC7634DL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x545E3872760AB7L,0xF25DBF55296C3AL,0xE082542A385502L,0x8BA79B9859F741L,0x20AD746E1D3B62L,0x5378EB1C71EF3L,0xAA87CA22BE8BL};
	public static final long[] CURVE_Gy= {0x431D7C90EA0E5FL,0xB1CE1D7E819D7AL,0x13B5F0B8C00A60L,0x289A147CE9DA31L,0x92DC29F8F41DBDL,0x2C6F5D9E98BF92L,0x3617DE4A9626L};


}

