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


package org.miracl.core.NIST256;

public class ROM
{

// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFL,0x0L,0x1000000L,0xFFFFFFFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFFFEL,0xFFFFFFFFFFL,0x0L,0x1000000L,0xFFFFFFFFL};
	public static final long[] R2modp= {0x3000000050000L,0x0L,0xFFFFFBFFFFFFFAL,0xFFFAFFFFFFFEFFL,0x2FFFFL};
	public static final long MConst= 0x1L;

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 0;
	public static final long[] CURVE_B= {0xCE3C3E27D2604BL,0x6B0CC53B0F63BL,0x55769886BC651DL,0xAA3A93E7B3EBBDL,0x5AC635D8L};
	public static final long[] CURVE_Order= {0xB9CAC2FC632551L,0xFAADA7179E84F3L,0xFFFFFFFFFFBCE6L,0xFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Gx= {0xA13945D898C296L,0x7D812DEB33A0F4L,0xE563A440F27703L,0xE12C4247F8BCE6L,0x6B17D1F2L};
	public static final long[] CURVE_Gy= {0xB6406837BF51F5L,0x33576B315ECECBL,0x4A7C0F9E162BCEL,0xFE1A7F9B8EE7EBL,0x4FE342E2L};

}

