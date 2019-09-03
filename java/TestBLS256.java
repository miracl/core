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

/* test driver and function exerciser for BLS API Functions */

/* To reverse the groups G1 and G2, edit BLS*.go

Swap G1 <-> G2
Swap ECP <-> ECPn
Disable G2 precomputation
Switch G1/G2 parameter order in pairing function calls

Swap G1S and G2S in this program

*/

package org.miracl.core.XXX;  //

import junit.framework.TestCase;      //
import org.miracl.core.RAND;

public class TestBLS256 extends TestCase { //
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testBLS256() {
        RAND rng = new RAND();
        int BGS = BLS256.BGS;
        int BFS = BLS256.BFS;
        int G1S = BFS + 1; /* Group 1 Size compressed */
        int G2S = 8 * BFS + 1; /* Group 2 Size compressed */

        byte[] S = new byte[BGS];
        byte[] W = new byte[G2S];
        byte[] SIG = new byte[G1S];

        byte[] RAW = new byte[100];

        rng.clean();
        for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);

        System.out.println("\nTesting BLS code");

        int res = BLS256.init();
        if (res != 0)
            fail("Failed to initialize");

        String mess = new String("This is a test message");

        res = BLS256.KeyPairGenerate(rng, S, W);
        if (res != 0)
            fail("Failed to Generate Keys");
        System.out.print("Private key : 0x");  printBinary(S);
        System.out.print("Public  key : 0x");  printBinary(W);

        BLS256.sign(SIG, mess, S);
        System.out.print("Signature : 0x");  printBinary(SIG);

        res = BLS256.verify(SIG, mess, W);

        if (res == 0)
            System.out.println("Signature is OK");
        else
            fail("Signature is *NOT* OK");

    }
}
