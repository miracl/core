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


package org.miracl.core.ED25519;

public class ROM
{

public static final long[] Modulus= {0xFFFFFFFFFFFFEDL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFFL};
public static final long[] R2modp= {0xA4000000000000L,0x5L,0x0L,0x0L,0x0L};
public static final long MConst= 0x13;


public static final int CURVE_Cof_I= 8;
public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_A= -1;
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0xEB4DCA135978A3L,0xA4D4141D8AB75L,0x797779E8980070L,0x2B6FFE738CC740L,0x52036CEEL};
public static final long[] CURVE_Order= {0x12631A5CF5D3EDL,0xF9DEA2F79CD658L,0x14DEL,0x0L,0x10000000L};
public static final long[] CURVE_Gx= {0x562D608F25D51AL,0xC7609525A7B2C9L,0x31FDD6DC5C692CL,0xCD6E53FEC0A4E2L,0x216936D3L};
public static final long[] CURVE_Gy= {0x66666666666658L,0x66666666666666L,0x66666666666666L,0x66666666666666L,0x66666666L};

}

