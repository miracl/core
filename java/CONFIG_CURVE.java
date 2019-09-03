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

public class CONFIG_CURVE {
    public static final int WEIERSTRASS = 0;
    public static final int EDWARDS = 1;
    public static final int MONTGOMERY = 2;
    public static final int NOT = 0;
    public static final int BN = 1;
    public static final int BLS = 2;
    public static final int D_TYPE = 0;
    public static final int M_TYPE = 1;
    public static final int POSITIVEX = 0;
    public static final int NEGATIVEX = 1;

    public static final int CURVETYPE = @CT@;
    public static final int CURVE_PAIRING_TYPE = @PF@;
    public static final int SEXTIC_TWIST = @ST@;
    public static final int SIGN_OF_X = @SX@;
    public static final int ATE_BITS = @AB@;
    public static final int G2_TABLE = @G2@;


    public static final int SHA256 = 32;
    public static final int SHA384 = 48;
    public static final int SHA512 = 64;

    public static final int HASH_TYPE = @HT@;
    public static final int AESKEY = @AK@;

    public static final boolean USE_GLV = true;
    public static final boolean USE_GS_G2 = true;
    public static final boolean USE_GS_GT = true;
}
