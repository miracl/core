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


package org.miracl.core.SECP160R1;

public class ROM
{
// Base Bits= 56
public static final long[] Modulus= {0xFFFFFF7FFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
public static final long[] R2modp= {0x1000000010000L,0x400000L,0x0L};
public static final long[] ROI= {0xFFFFFF7FFFFFFEL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFL};
public static final long MConst= 0xFFFFFF80000001L;

public static final int CURVE_A= -3;
public static final int CURVE_Cof_I= 1;
public static final long[] CURVE_Cof= {0x1L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0xD4D4ADC565FA45L,0x7A8B65ACF89F81L,0x1C97BEFC54BDL};
public static final long[] CURVE_Order= {0x27AED3CA752257L,0x1F4C8F9L,0x1000000000000L};
public static final long[] CURVE_Gx= {0xC38BB913CBFC82L,0x73284664698968L,0x4A96B5688EF5L};
public static final long[] CURVE_Gy= {0x2351377AC5FB32L,0x947D59DCC91204L,0x23A628553168L};
}

