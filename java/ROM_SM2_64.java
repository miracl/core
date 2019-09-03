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


package org.miracl.core.SM2;

public class ROM
{

// Base Bits= 56
public static final long[] Modulus= {0xFFFFFFFFFFFFFFL,0xFFFF00000000FFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
public static final long[] R2modp= {0x3000000040000L,0xFFFFFFFE000000L,0x300000002L,0x10000000100L,0x60000L};
public static final long MConst= 0x1L;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_A= -3;
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0xBCBD414D940E93L,0x89F515AB8F92DDL,0x4BCF6509A7F397L,0x9D9F5E344D5A9EL,0x28E9FA9EL};
public static final long[] CURVE_Order= {0xBBF40939D54123L,0xDF6B21C6052B53L,0xFFFFFFFFFF7203L,0xFFFFFFFFFFFFFFL,0xFFFFFFFEL};
public static final long[] CURVE_Gx= {0x5A4589334C74C7L,0xBBFF2660BE171L,0x466A39C9948FE3L,0x1F1981195F9904L,0x32C4AE2CL};
public static final long[] CURVE_Gy= {0xDF32E52139F0A0L,0x877CC62A474002L,0xE36B692153D0A9L,0xF4F6779C59BDCEL,0xBC3736A2L};
}

