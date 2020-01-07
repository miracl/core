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


package org.miracl.core.C1174;

public class ROM
{

// Base Bits= 56
public static final long[] Modulus= {0xFFFFFFFFFFFFF7L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long[] R2modp= {0x0L,0x144L,0x0L,0x0L,0x0L};
public static final long[] ROI= {0xFFFFFFFFFFFFF6L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long MConst= 0x9L;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_A= 1;
public static final int CURVE_Cof_I= 4;
public static final long[] CURVE_Cof= {0x4L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= -1174;
public static final long[] CURVE_B= {0xFFFFFFFFFFFB61L,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0xFFFFFFFFFFFFFFL,0x7FFFFFFL};
public static final long[] CURVE_Order= {0x44D45FD166C971L,0x65C4DFD3073489L,0xFFFFFFFFFFF779L,0xFFFFFFFFFFFFFFL,0x1FFFFFFL};
public static final long[] CURVE_Gx= {0x123F27BCE29EDAL,0xD96A492ECD6516L,0xE7C029A190C021L,0xEA308C479343AEL,0x37FBB0CL};
public static final long[] CURVE_Gy= {0xCCB1BF9B46360EL,0xDEE2AF3F976BA4L,0x1169840E0C4FE2L,0xD47FB7CC665684L,0x6B72F82L};
}

