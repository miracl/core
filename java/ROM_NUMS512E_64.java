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


package org.miracl.core.NUMS512E;

public class ROM
{
// Base Bits= 56
	public static final long[] Modulus= {0xFFFFFFFFFFFDC7L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFL};
	public static final long[] ROI= {0xFFFFFFFFFFFDC6L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFL};
	public static final long[] R2modp= {0x0L,0xF0B10000000000L,0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x239L;

	public static final int CURVE_Cof_I= 4;
	public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= -78296;
	public static final long[] CURVE_B= {0xFFFFFFFFFECBEFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFL};
	public static final long[] CURVE_Order= {0x468CF51BEED46DL,0x5786DEFECFF67L,0xC970B686F52A46L,0x2FCF91BA9E3FD8L,0xFFFFFFB4F0636DL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x3FL};
	public static final long[] CURVE_Gx= {0xB9AB2999EC57FEL,0x25427CC4F015C5L,0x92568904AD0FE5L,0xEE46730F78BDC9L,0x3B81474621C14EL,0xA38227A17EBE27L,0x332FD1E79F4DC4L,0x7A18CB7888D3C5L,0x8E316D128DB69CL,0xDFL};
	public static final long[] CURVE_Gy= {0x6DDEC0C1E2F5E1L,0xD38A9BF1D01F32L,0x862AECC1FD0266L,0xE9963562601A06L,0x9E834120CA53F2L,0x9D22A92B6B9590L,0x6EE476F726D825L,0x98B0F577A82A25L,0x9BFF39D49CA71L,0x6DL};
}
