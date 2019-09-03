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

/* Boneh-Lynn-Shacham signature API Functions */

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.SHA3;

public class BLS {
    public static final int BFS = CONFIG_BIG.MODBYTES;
    public static final int BGS = CONFIG_BIG.MODBYTES;
    public static final int BLS_OK = 0;
    public static final int BLS_FAIL = -1;

    public static FP4[] G2_TAB;


    /* hash a message to an ECP point, using SHA3 */

    public static ECP bls_hashit(String m) {
        SHA3 sh = new SHA3(SHA3.SHAKE256);
        byte[] hm = new byte[BFS];
        byte[] t = m.getBytes();
        for (int i = 0; i < t.length; i++)
            sh.process(t[i]);
        sh.shake(hm, BFS);
        ECP P = ECP.mapit(hm);
        return P;
    }

    public static int init() {
        ECP2 G = ECP2.generator();
        if (G.is_infinity()) return BLS_FAIL;
        G2_TAB = PAIR.precomp(G);
        return BLS_OK;
    }

    /* generate key pair, private key S, public key W */

    public static int KeyPairGenerate(RAND RNG, byte[] S, byte[] W) {
        ECP2 G = ECP2.generator();
        if (G.is_infinity()) return BLS_FAIL;
        BIG q = new BIG(ROM.CURVE_Order);
        BIG s = BIG.randtrunc(q, 16 * CONFIG_CURVE.AESKEY, RNG);
        s.toBytes(S);
        G = PAIR.G2mul(G, s);
        G.toBytes(W,true);
        return BLS_OK;
    }

    /* Sign message m using private key S to produce signature SIG */

    public static int sign(byte[] SIG, String m, byte[] S) {
        ECP D = bls_hashit(m);
        BIG s = BIG.fromBytes(S);
        D = PAIR.G1mul(D, s);
        D.toBytes(SIG, true);
        return BLS_OK;
    }

    /* Verify signature given message m, the signature SIG, and the public key W */

    public static int verify(byte[] SIG, String m, byte[] W) {
        ECP HM = bls_hashit(m);

        ECP D = ECP.fromBytes(SIG);
        if (!PAIR.G1member(D)) return BLS_FAIL;
        D.neg();
        ECP2 PK = ECP2.fromBytes(W);

// Use new multi-pairing mechanism
        FP12[] r = PAIR.initmp();
        PAIR.another_pc(r, G2_TAB, D);
        PAIR.another(r, PK, HM);
        FP12 v = PAIR.miller(r);

//.. or alternatively
//		ECP2 G=ECP2.generator();
//		if (G.is_infinity()) return BLS_FAIL;
//		FP12 v=PAIR.ate2(G,D,PK,HM);

        v = PAIR.fexp(v);
        if (v.isunity())
            return BLS_OK;
        return BLS_FAIL;
    }
}
