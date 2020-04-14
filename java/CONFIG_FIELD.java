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

package org.miracl.core.XXX;

public class CONFIG_FIELD {
    public static final int NOT_SPECIAL = 0;
    public static final int PSEUDO_MERSENNE = 1;
    public static final int MONTGOMERY_FRIENDLY = 2;
    public static final int GENERALISED_MERSENNE = 3;

    public static final int NEGATOWER = 0;
    public static final int POSITOWER = 1;

    public static final int MODBITS = @NBT@; /* Number of bits in Modulus */
    public static final int PM1D2 = @M8@; /* Modulus mod 8 */
    public static final int MODTYPE = @MT@;
    public static final int QNRI = @QI@;
    public static final int RIADZ = @RZ@;
    public static final int TOWER = @TW@;

    public static final boolean BIG_ENDIAN_SIGN = false;

    public static final int FEXCESS = (((int)1 << @SH@) - 1); // BASEBITS*NLEN-MODBITS or 2^30 max!
}
