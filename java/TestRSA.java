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

/* test driver and function exerciser for ECDH/ECIES/ECDSA API Functions */
package org.miracl.core.XXX;

import java.util.Scanner;
import junit.framework.TestCase;
import org.miracl.core.RAND;

public class TestRSA extends TestCase {
    private static void printBinary(byte[] array) {
        int i;
        for (i = 0; i < array.length; i++) {
            System.out.printf("%02x", array[i]);
        }
        System.out.println();
    }

    public static void testRSA() {
        byte[] RAW = new byte[100];
        RAND rng = new RAND();

        rng.clean();
        for (int i = 0; i < 100; i++) RAW[i] = (byte)(i);

        rng.seed(100, RAW);

        int i;
        int RFS = RSA.RFS;
        int sha = RSA.HASH_TYPE;

        String message = "Hello World\n";

        public_key pub = new public_key(CONFIG_FF.FFLEN);
        private_key priv = new private_key(FF.HFLEN);

        byte[] ML = new byte[RFS];
        byte[] C = new byte[RFS];
        byte[] S = new byte[RFS];


        System.out.println("Testing RSA code");

        System.out.println("Generating public/private key pair");
        RSA.KEY_PAIR(rng, 65537, priv, pub);

        byte[] M = message.getBytes();
        System.out.print("Encrypting test string\n");


        byte[] E = RSA.OAEP_ENCODE(sha, M, rng, null); /* OAEP encode message M to E  */

        if (E.length == 0) {
            fail("Encoding failed");
        }

        RSA.ENCRYPT(pub, E, C);   /* encrypt encoded message */
        System.out.print("Ciphertext= 0x"); printBinary(C);

        System.out.print("Decrypting test string\n");
        RSA.DECRYPT(priv, C, ML);
        byte[] MS = RSA.OAEP_DECODE(sha, null, ML); /* OAEP decode message  */

        message = new String(MS);
        System.out.print(message);

        System.out.println("Signing message");
        RSA.PKCS15(sha, M, C);

        RSA.DECRYPT(priv, C, S); /* create signature in S */

        System.out.print("Signature= 0x"); printBinary(S);

        RSA.ENCRYPT(pub, S, ML);

        boolean cmp = true;
        if (C.length != ML.length) cmp = false;
        else {
            for (int j = 0; j < C.length; j++)
                if (C[j] != ML[j]) cmp = false;
        }
        if (cmp) System.out.println("Signature is valid");
        else fail("Signature is INVALID");

        RSA.PRIVATE_KEY_KILL(priv);
        System.out.println("");

    }
}
