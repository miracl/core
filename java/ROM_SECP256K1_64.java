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


package org.miracl.core.SECP256K1;

public class ROM
{

// Base Bits= 56
public static final long[] Modulus= {0xFFFFFEFFFFFC2FL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] R2modp= {0xA1000000000000L,0x7A2000E90L,0x1L,0x0L,0x0L};
public static final long MConst= 0x38091DD2253531L;

public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_A= 0;
public static final int CURVE_B_I= 7;
public static final long[] CURVE_B= {0x7L,0x0L,0x0L,0x0L,0x0L};
public static final long[] CURVE_Order= {0xD25E8CD0364141L,0xDCE6AF48A03BBFL,0xFFFFFFFFFEBAAEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFL};
public static final long[] CURVE_Gx= {0xF2815B16F81798L,0xFCDB2DCE28D959L,0x95CE870B07029BL,0xF9DCBBAC55A062L,0x79BE667EL};
public static final long[] CURVE_Gy= {0x47D08FFB10D4B8L,0xB448A68554199CL,0xFC0E1108A8FD17L,0x26A3C4655DA4FBL,0x483ADA77L};

}

