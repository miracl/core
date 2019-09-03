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

/* test driver and function exerciser for NewHope API Functions */

// See https://eprint.iacr.org/2016/1157 (Alkim, Ducas, Popplemann and Schwabe)

package org.miracl.core;

import java.util.Scanner;
import junit.framework.TestCase;       //
import org.miracl.core.RAND;
import org.miracl.core.NHS;

public class TestNHS extends TestCase {
    public static void testNHS() {
        int i;
        byte[] RAW = new byte[100];
        byte[] S = new byte[1792];
        byte[] SB = new byte[1824];
        byte[] UC = new byte[2176];
        byte[] KEYA = new byte[32];
        byte[] KEYB = new byte[32];

        System.out.println("\nTesting New Hope Key Exchange");

        RAND SRNG = new RAND();
        RAND CRNG = new RAND();
        SRNG.clean(); CRNG.clean();

        for (i = 0; i < 100; i++) RAW[i] = (byte)(i + 1);
        SRNG.seed(100, RAW);

        for (i = 0; i < 100; i++) RAW[i] = (byte)(i + 2);
        CRNG.seed(100, RAW);

// NewHope Simple key exchange

        NHS.SERVER_1(SRNG, SB, S);
        NHS.CLIENT(CRNG, SB, UC, KEYB);
        NHS.SERVER_2(S, UC, KEYA);

        System.out.printf("Alice key= 0x");
        for (i = 0; i < KEYA.length; i++)
            System.out.printf("%02x", KEYA[i]);
        System.out.println();


        System.out.printf("Bob's key= 0x");
        for (i = 0; i < KEYA.length; i++)
            System.out.printf("%02x", KEYB[i]);
        System.out.println();

        boolean ok=true;
        if (KEYA.length!=KEYB.length) ok=false;
            
        for (i=0;i<KEYA.length;i++ )
        {
            if (KEYA[i]!=KEYB[i]) ok=false;
        }
        if (!ok)
            fail("Keys are not the same!");

    }
}
