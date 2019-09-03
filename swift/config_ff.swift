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

public struct CONFIG_FF{
    static public let FFLEN:UInt=@ML@

    static public let FF_BITS:UInt=(CONFIG_BIG.BIGBITS*CONFIG_FF.FFLEN) /* Finite Field Size in bits - must be BIGBITS.2^n */
    static public let HFLEN=(CONFIG_FF.FFLEN/2);  /* Useful for half-size RSA private key operations */

    static let P_MBITS:UInt=CONFIG_BIG.MODBYTES*8
    static let P_OMASK:Chunk=Chunk(-1)<<Chunk(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
    static let P_FEXCESS:Chunk=(1<<Chunk(CONFIG_BIG.BASEBITS*UInt(CONFIG_BIG.NLEN)-CONFIG_FF.P_MBITS-1))
    static let P_TBITS=(CONFIG_FF.P_MBITS%CONFIG_BIG.BASEBITS)
}
