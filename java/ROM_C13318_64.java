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


package org.miracl.core.C13318;

public class ROM
{

//*** rom field parameters*****
// Base Bits= 56
public static final long[] Modulus= {0xFFFFFFFFFFFFEDL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFFL};
public static final long[] R2modp= {0xA4000000000000L,0x5L,0x0L,0x0L,0x0L};
public static final long[] ROI= {0xEE1B274A0EA0B0L,0x1806AD2FE478C4L,0x993DFBD7A72F43L,0x4FC1DF0B2B4D00L,0x2B832480L};
public static final long MConst= 0x13L;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_A= -3;
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 13318;
public static final long[] CURVE_B= {0x3406L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0x7D12DC4DC2CBE3L,0x54F83DEB8D16EBL,0xF4F6L,0x0L,0x80000000L};
public static final long[] CURVE_Gx= {0x5L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Gy= {0xC8D3546B6EAD0BL,0xCB04CF016DB5B8L,0xE5166966369D4CL,0x26BCA6F1381630L,0x6675AAD9L};

}

