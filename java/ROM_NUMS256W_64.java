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


package org.miracl.core.NUMS256W;

public class ROM
{

// Modulus
	public static final long[] Modulus= {0xFFFFFFFFFFFF43L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] R2modp= {0x89000000000000L,0x8BL,0x0L,0x0L,0x0L};
	public static final long MConst= 0xBDL;

// Curve

	public static final int CURVE_Cof_I= 1;
	public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= -3;
	public static final int CURVE_B_I= 152961;
	public static final long[] CURVE_B= {0x25581L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xAB20294751A825L,0x8275EA265C6020L,0xFFFFFFFFFFE43CL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x52EE1EB21AACB1L,0x9B0903D4C73ABCL,0xA04F42CB098357L,0x5AAADB61297A95L,0xBC9ED6B6L};
	public static final long[] CURVE_Gy= {0xB5B9CB2184DE9FL,0xC3D115310FBB80L,0xF77E04E035C955L,0x3399B6A673448BL,0xD08FC0F1L};

}

