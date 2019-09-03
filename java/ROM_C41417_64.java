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


package org.miracl.core.C41417;

public class ROM
{

// Base Bits= 60
	public static final long[] Modulus= {0xFFFFFFFFFFFFFEFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFL};
	public static final long[] R2modp= {0x121000L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long MConst= 0x11L;


	public static final int CURVE_Cof_I= 8;
	public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final int CURVE_A= 1;
	public static final int CURVE_B_I= 3617;
	public static final long[] CURVE_B= {0xE21L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
	public static final long[] CURVE_Order= {0xB0E71A5E106AF79L,0x1C0338AD63CF181L,0x414CF706022B36FL,0xFFFFFFFFEB3CC92L,0xFFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFFL,0x7FFFFFFFFFFFFL};
	public static final long[] CURVE_Gx= {0x4FD3812F3CBC595L,0x1A73FAA8537C64CL,0x4AB4D6D6BA11130L,0x3EC7F57FF35498AL,0xE5FCD46369F44C0L,0x300218C0631C326L,0x1A334905141443L};
	public static final long[] CURVE_Gy= {0x22L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};

}

