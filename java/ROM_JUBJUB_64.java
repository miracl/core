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


package org.miracl.core.JUBJUB;

public class ROM
{

//*** rom field parameters*****
// Base Bits= 56
public static final long[] Modulus= {0xFFFFFF00000001L,0xA402FFFE5BFEFFL,0x809A1D80553BDL,0x299D7D483339D8L,0x73EDA753L};
public static final long[] ROI= {0x788F500B912F1FL,0x4FF270B3E0941BL,0xC8D168D6C0C402L,0x5B416B6F0FD56DL,0x212D79EL};
public static final long[] R2modp= {0x3B3440EC31BBA9L,0x8929657E045FB0L,0x2D645CF57C6E1AL,0xEA6A1C5012ECF5L,0x3C7B9D12L};

public static final long MConst= 0xFFFFFEFFFFFFFFL;

//*** rom curve parameters *****
// Base Bits= 56

public static final int CURVE_A= -1;
public static final int CURVE_Cof_I= 8;
public static final long[] CURVE_Cof= {0x8L,0x0L,0x0L,0x0L,0x0L};
public static final int CURVE_B_I= 0;
public static final long[] CURVE_B= {0x65FD6D6343EB1L,0x7F6D37579D2601L,0x7E6BD7FD4292DL,0x4BFA2B48F5FD92L,0x2A9318E7L};
public static final long[] CURVE_Order= {0x970E5ED6F72CB7L,0x2093CCC81082D0L,0x101343B00A668L,0x6533AFA906673BL,0xE7DB4EAL};
public static final long[] CURVE_Gx= {0x4ECF1A74F976C4L,0x546BF2F39EDE7FL,0xDF00384882000CL,0xF8EFF38CA624B4L,0x5183972AL};
public static final long[] CURVE_Gy= {0x6BAD709349702EL,0x8707FFA6833B14L,0x5ABD9DC308096CL,0x2CA2FC2C9E8FCCL,0x3B43F847L};

}

