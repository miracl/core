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


package org.miracl.core.GOLDILOCKS;

public class ROM
{

// Base Bits= 58
public static final long[] Modulus= {0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
public static final long[] R2modp= {0x200000000L,0x0L,0x0L,0x0L,0x3000000L,0x0L,0x0L,0x0L};
public static final long MConst= 0x1L;


public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_A= 1;
public static final int CURVE_B_I= -39081;
public static final long[] CURVE_B= {0x3FFFFFFFFFF6756L,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FBFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFL};
public static final long[] CURVE_Order= {0x378C292AB5844F3L,0x3309CA37163D548L,0x1B49AED63690216L,0x3FDF3288FA7113BL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0x3FFFFFFFFFFFFFFL,0xFFFFFFFFFFL};
public static final long[] CURVE_Gx= {0x155555555555555L,0x155555555555555L,0x155555555555555L,0x2A5555555555555L,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAAAAAL,0x2AAAAAAAAAAL};
public static final long[] CURVE_Gy= {0x2EAFBCDEA9386EDL,0x32CAFB473681AF6L,0x25833A2A3098BBBL,0x1CA2B6312E03595L,0x35884DD7B7E36DL,0x21B0AC00DBB5E8L,0x17048DB359D6205L,0x2B817A58D2BL};

}

