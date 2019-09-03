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

/*
 * Implementation of the Secure Hashing Algorithm (SHA-256)
 *
 * Generates a 256 bit message digest. It should be impossible to come
 * come up with two messages that hash to the same value ("collision free").
 *
 * For use with byte-oriented messages only.
 */

package org.miracl.core;

import org.miracl.core.HASH256;
import org.miracl.core.HASH384;
import org.miracl.core.HASH512;
import org.miracl.core.SHA3;

public class HMAC {

    public static final int MC_SHA2 = 2;
    public static final int MC_SHA3 = 3;

    /* Convert Integer to n-byte array */
    public static byte[] inttoBytes(int n, int len) {
        int i;
        byte[] b = new byte[len];

        for (i = 0; i < len; i++) b[i] = 0;
        i = len;
        while (n > 0 && i > 0) {
            i--;
            b[i] = (byte)(n & 0xff);
            n /= 256;
        }
        return b;
    }

    public static byte[] GPhashit(int hash, int sha, int pad, byte[] A, int n, byte[] B) {
        byte[] R = null;

        if (hash == MC_SHA2)
        {
            if (sha == 32) {
                HASH256 H = new HASH256();
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
            if (sha == 48) {
                HASH384 H = new HASH384();
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
            if (sha == 64) {
                HASH512 H = new HASH512();
                if (A != null) H.process_array(A); 
                if (n >= 0) H.process_num(n);
                if (B != null) H.process_array(B);
                R = H.hash();
            }
        }
        if (hash == MC_SHA3)
        {
            SHA3 H = new SHA3(sha);
            if (A != null) H.process_array(A); 
            if (n >= 0) H.process_num(n);
            if (B != null) H.process_array(B);
            R = H.hash();
        }
        if (R == null) return null;

        if (pad == 0) return R;
        /* If pad>0 output is truncated or padded to pad bytes */
        byte[] W = new byte[pad];
        if (pad <= sha) {
            for (int i = 0; i < pad; i++) W[i] = R[i];
        } else {
            for (int i = 0; i < sha; i++) W[i + pad - sha] = R[i];
            for (int i = 0; i < pad - sha; i++) W[i] = 0;
        }
        return W;
    }

    public static byte[] SPhashit(int hash, int hlen, byte[] A)
    {
        return GPhashit(hash, hlen, 0, A, -1, null);
    }

    public static byte[] KDF2(int hash, int sha, byte[] Z, byte[] P, int olen) {
        /* NOTE: the parameter olen is the length of the output k in bytes */
        int hlen = sha;
        byte[] K = new byte[olen];
        byte[] B;
        int counter, cthreshold, k = 0;

        for (int i = 0; i < K.length; i++) K[i] = 0;

        cthreshold = olen / hlen; if (olen % hlen != 0) cthreshold++;

        for (counter = 1; counter <= cthreshold; counter++) {
            B = GPhashit(hash,sha, 0, Z, counter, P);
            if (k + hlen > olen) for (int i = 0; i < olen % hlen; i++) K[k++] = B[i];
            else for (int i = 0; i < hlen; i++) K[k++] = B[i];
        }

        return K;
    }

    /* Password based Key Derivation Function */
    /* Input password p, salt s, and repeat count */
    /* Output key of length olen */
    public static byte[] PBKDF2(int hash, int sha, byte[] Pass, byte[] Salt, int rep, int olen) {
        int i, j, k, len, d, opt;
        d = olen / sha; if (olen % sha != 0) d++;
        byte[] F = new byte[sha];
        byte[] U = new byte[sha];
        byte[] S = new byte[Salt.length + 4];

        byte[] K = new byte[d * sha];
        opt = 0;

        for (i = 1; i <= d; i++) {
            for (j = 0; j < Salt.length; j++) S[j] = Salt[j];
            byte[] N = inttoBytes(i, 4);
            for (j = 0; j < 4; j++) S[Salt.length + j] = N[j];

            HMAC1(hash, sha, F, sha, Pass, S);

            for (j = 0; j < sha; j++) U[j] = F[j];
            for (j = 2; j <= rep; j++) {
                HMAC1(hash, sha, U, sha, Pass, U);
                for (k = 0; k < sha; k++) F[k] ^= U[k];
            }
            for (j = 0; j < sha; j++) K[opt++] = F[j];
        }
        byte[] key = new byte[olen];
        for (i = 0; i < olen; i++) key[i] = K[i];
        return key;
    }

    /* Calculate HMAC of m using key k. HMAC is tag of length olen */
    public static int HMAC1(int hash, int sha, byte[] tag, int olen, byte[] K, byte[] M ) {
        /* Input is from an octet m        *
        * olen is requested output length in bytes. k is the key  *
        * The output is the calculated tag */
        
        int b=0;
        if (hash == MC_SHA2)
        {
            b=64;
            if (sha > 32)
                b=128;
        }
        if (hash == MC_SHA3)
        {
            b=200-2*sha;
        }
        if (b==0) return 0;

        byte[] B;
        byte[] K0 = new byte[b];
        
        for (int i = 0; i < b; i++) K0[i] = 0;

        if (K.length > b) {
            B = SPhashit(hash,sha,K);
            for (int i = 0; i < sha; i++) K0[i] = B[i];
        } else
            for (int i = 0; i < K.length; i++ ) K0[i] = K[i];

        for (int i = 0; i < b; i++) K0[i] ^= 0x36;
        B = GPhashit(hash, sha, 0, K0, -1, M);

        for (int i = 0; i < b; i++) K0[i] ^= 0x6a;
        B = GPhashit(hash, sha, olen, K0, -1, B);

        for (int i = 0; i < olen; i++) tag[i] = B[i];

        return 1;
    }

    public static byte[] HKDF_Extract(int hash, int hlen, byte[] SALT, byte[] IKM)  { 
        byte[] PRK = new byte[hlen];
	    if (SALT == null) {
		    byte[] H = new byte[hlen];
		    for (int i = 0; i < hlen; i++) H[i]=0;
		    HMAC1(hash,hlen,PRK,hlen,H,IKM);
	    } else {
		    HMAC1(hash,hlen,PRK,hlen,SALT,IKM);
	    }
        return PRK;
    }

    public static byte[] HKDF_Expand(int hash, int hlen, int olen, byte[] PRK, byte[] INFO) { 
	    int i,j,k,m,n = olen/hlen;
	    int flen = olen%hlen;

	    byte[] OKM = new byte[olen];
	    byte[] T = new byte[1+INFO.length];
	    byte[] K = new byte[hlen];

        k=m=0;
	    for (i=1;i<=n;i++) {
		    for (j = 0; j < INFO.length; j++)
			    T[k++] = INFO[j];
		    T[k++]=(byte)i;
		    HMAC1(hash,hlen,K,hlen,PRK,T);
		    k=0;
            if (i==1) T = new byte[INFO.length+1+hlen]; // resize T
		    for (j = 0; j < hlen; j++) {
			    OKM[m++] = K[j];
			    T[k++] = K[j];
		    }
	    }
	    if (flen > 0) {
		    for (j = 0; j < INFO.length; j++) 
			    T[k++] = INFO[j];
		    T[k++] = (byte)(n+1);
		    HMAC1(hash,hlen,K,flen,PRK,T);
		    for (j = 0; j < flen; j++) 
			    OKM[m++] = K[j];
		    
	    }
	    return OKM;
    }   
}


/*
        byte[] ikm=new byte[22];
        byte[] salt=new byte[13];
        byte[] info=new byte[10];
     
        for (i=0;i<22;i++) ikm[i]=0x0b;
        for (i=0;i<13;i++) salt[i]=(byte)i;
        for (i=0;i<10;i++) info[i]=(byte)(0xf0+(byte)i);

        byte[] prk=HMAC.HKDF_Extract(HMAC.MC_SHA2,32,salt,ikm);
        System.out.printf("PRK= ");
        for (i=0;i<prk.length ;i++ )
            System.out.printf("%02x",prk[i]);

        byte[] okm=HMAC.HKDF_Expand(HMAC.MC_SHA2,32,42,prk,info);
        System.out.printf("\nOKM= ");
        for (i=0;i<okm.length ;i++ )
            System.out.printf("%02x",okm[i]);
        System.out.println();

*/


