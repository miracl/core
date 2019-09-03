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

public struct CONFIG_FIELD{
    static public let NOT_SPECIAL=0
    static public let PSEUDO_MERSENNE=1
    static public let MONTGOMERY_FRIENDLY=2
    static public let GENERALISED_MERSENNE=3

    static public let NEGATOWER=0
    static public let POSITOWER=1

    static public let MODBITS:UInt = @NBT@
    static let MOD8:UInt = @M8@
    static public let MODTYPE =  @MT@   
    static public let QNRI =  @QI@   
    static public let TOWER = @TW@

    static let FEXCESS:Int32 = ((Int32(1)<<@SH@)-1)
    static let OMASK:Chunk=Chunk(-1)<<Chunk(CONFIG_FIELD.MODBITS%CONFIG_BIG.BASEBITS)
    static let TBITS:UInt=CONFIG_FIELD.MODBITS%CONFIG_BIG.BASEBITS; // Number of active bits in top word
    static let TMASK:Chunk=(1<<Chunk(CONFIG_FIELD.TBITS))-1

}