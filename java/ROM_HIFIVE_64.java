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


package org.miracl.core.HIFIVE;

public class ROM
{

// Base Bits= 60
	public static final long[] Modulus= {0xFFFFFFFFFFFFFFDL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFL};
	public static final long[] R2modp= {0x9000000000000L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x3L;

	public static final int CURVE_Cof_I= 8;
	public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= 11111;
	public static final long[] CURVE_B= {0x2B67L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xB2F95973E9FA805L,0xC0BD6B87F93BAA7L,0x71415FA9850L,0x0L,0x0L,0x200000000L};
	public static final long[] CURVE_Gx= {0xCL,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Gy= {0x2BEC68505FE8632L,0x5D5650CA0365DB1L,0x3811C7EF435B6DBL,0x7853D1B14B46CL,0x56502E18E1C161DL,0xC0DC616BL};

}

