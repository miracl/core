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

/* test driver and function exerciser for MPIN API Functions */
package org.miracl.core.XXX;

import java.util.Scanner;
import junit.framework.TestCase;
import org.miracl.core.RAND;

public class TesttimeMPIN192 extends TestCase {
    public static final int MIN_TIME = 10; /* seconds */
    public static final int MIN_ITERS = 10;

    static boolean PERMITS = true;
    static boolean PINERROR = true;
    static boolean FULL = true;
    static boolean SINGLE_PASS = false;

    public static void testtimeMPIN() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();

        int i, iterations;
        long start, elapsed;
        double dur;

        rng.clean();
        for (i = 0; i < 100; i++) RAW[i] = (byte)(i);
        rng.seed(100, RAW);


        System.out.println("\nTesting/Timing Pairings");
        if (CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BN) {
            System.out.print("BN Pairing-Friendly Curve\n");
        }
        if (CONFIG_CURVE.CURVE_PAIRING_TYPE == CONFIG_CURVE.BLS) {
            System.out.print("BLS24 Pairing-Friendly Curve\n");
        }

        System.out.format("Modulus size %d bits\n", CONFIG_FIELD.MODBITS);
        System.out.format("%d bit build\n", BIG.CHUNK);

        ECP G = ECP.generator();

        BIG r = new BIG(ROM.CURVE_Order);
        BIG s = BIG.randtrunc(r, 16 * CONFIG_CURVE.AESKEY, rng);

        ECP P = PAIR192.G1mul(G, r);

        if (!P.is_infinity()) {
            fail("FAILURE - rP!=O");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            P = PAIR192.G1mul(G, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("G1 mul              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        ECP4 Q = ECP4.generator();
        ECP4 W = PAIR192.G2mul(Q, r);

        if (!W.is_infinity()) {
            fail("FAILURE - rQ!=O");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            W = PAIR192.G2mul(Q, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("G2 mul              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        FP24 w = PAIR192.ate(Q, P);
        w = PAIR192.fexp(w);

        FP24 g = PAIR192.GTpow(w, r);

        if (!g.isunity()) {
            fail("FAILURE - g^r!=1");
        }

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            g = PAIR192.GTpow(w, s);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("GT pow              - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);
/*
        g.copy(w);
        FP8 cr;
        iterations = 0;
        start = System.currentTimeMillis();
        do {
            cr = g.compow(s, r);

            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("GT pow (compressed) - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);
*/
        iterations = 0;
        start = System.currentTimeMillis();
        do {
            w = PAIR192.ate(Q, P);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("PAIRing ATE         - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        iterations = 0;
        start = System.currentTimeMillis();
        do {
            g = PAIR192.fexp(w);
            iterations++;
            elapsed = (System.currentTimeMillis() - start);
        } while (elapsed < MIN_TIME * 1000 || iterations < MIN_ITERS);
        dur = (double)elapsed / iterations;
        System.out.format("PAIRing FEXP        - %8d iterations  ", iterations);
        System.out.format(" %8.2f ms per iteration\n", dur);

        P.copy(G);
        Q.copy(W);

        P = PAIR192.G1mul(P, s);

        g = PAIR192.ate(Q, P);
        g = PAIR192.fexp(g);

        P.copy(G);
        Q = PAIR192.G2mul(Q, s);

        w = PAIR192.ate(Q, P);
        w = PAIR192.fexp(w);

        if (!PAIR192.G1member(P))
        {
            fail("FAILURE - P not in G1 ");
        }

        if (!PAIR192.G2member(Q))
        {
            fail("FAILURE - Q not in G2 ");
        }

        if (!PAIR192.GTmember(w))
        {
            fail("FAILURE - e(Q,P) not in GT ");
        }

        if (!g.equals(w)) {
            fail("FAILURE - e(sQ,p)!=e(Q,sP) ");
        }

        Q.copy(W);
        g = PAIR192.ate(Q, P);
        g = PAIR192.fexp(g);
        g = PAIR192.GTpow(g, s);

        if (!g.equals(w)) {
            fail("FAILURE - e(sQ,p)!=e(Q,P)^s ");
        }

        System.out.print("All tests pass\n");
    }

}
