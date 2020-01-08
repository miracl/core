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


package org.miracl.core.C1665;

public class ROM
{
// Base Bits= 60
public static final long[] Modulus= {0xFFFFFFFFFFFFFFBL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
public static final long[] R2modp= {0x190000000L,0x0L,0x0L};
public static final long[] ROI= {0xFFFFFFFFFFFFFFAL,0xFFFFFFFFFFFFFFFL,0x3FFFFFFFFFFFL};
public static final long MConst= 0x5L;

//*** rom curve parameters *****
// Base Bits= 60

public static final int CURVE_A= 1;
public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L};
public static final int CURVE_B_I= 5766;
public static final long[] CURVE_B= {0x1686L,0x0L,0x0L};
public static final long[] CURVE_Order= {0x80FF0A99DBA8B27L,0xFFFFFFFFFD5EF01L,0xFFFFFFFFFFFL};
public static final long[] CURVE_Gx= {0x671B9DBF9D52398L,0x9A0618EE0F666CL,0x14C94DA505B8L};
public static final long[] CURVE_Gy= {0xC7087B244920345L,0x13BFC7739D62DC2L,0x29414549BC0AL};
}

