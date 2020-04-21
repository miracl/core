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

/* Run through some test vectors for hash-to-curve draft standard */

package org.miracl.core.XXX;  //

import junit.framework.TestCase;       //
import org.miracl.core.HMAC;

public class TestHTP extends TestCase { 

    private static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

    private static FP[] hash_to_field(int hash,int hlen,byte[] DST,byte[] M,int ctr) {
		FP[] u=new FP[2];
        BIG q=new BIG(ROM.Modulus);
        int k=q.nbits();
        BIG r=new BIG(ROM.CURVE_Order);
        int m=r.nbits();
        int L=ceil(k+ceil(m,2),8);
        byte[] OKM = HMAC.XMD_Expand(hash,hlen,L*ctr,DST,M);
        byte[] fd = new byte[L]; 
        for (int i=0;i<ctr;i++)
        {
            for (int j=0;j<L;j++)
                fd[j]=OKM[i*L+j];
            DBIG dx=DBIG.fromBytes(fd);
            FP w=new FP(dx.mod(q));
            u[i]=new FP(w);
        }
        return u;
    }

    private static void htp(String mess,String ro,String nu) {
        System.out.println("\nRandom Access - message= "+mess);
        byte[] DSTRO = ro.getBytes();
        byte[] DSTNU = nu.getBytes();
        byte[] M = mess.getBytes();

        FP[] u=hash_to_field(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,DSTRO,M,2);
        System.out.println("u[0]= "+u[0].toString());
        System.out.println("u[1]= "+u[1].toString());

        ECP P=ECP.map2point(u[0]);
        System.out.println("Q[0]= "+P.toString());
        ECP P1=ECP.map2point(u[1]);
        System.out.println("Q[1]= "+P1.toString());
        P.add(P1);
        P.cfp();
        P.affine();
        System.out.println("P= "+P.toString());

        System.out.println("\nNon-Uniform");
        u=hash_to_field(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,DSTNU,M,1);
        System.out.println("u[0]= "+u[0].toString());
        P=ECP.map2point(u[0]);
        System.out.println("Q= "+P.toString());
        P.cfp();
        P.affine();
        System.out.println("P= "+P.toString());
    }

    public void testHTP() {
        String packageName = this.getClass().getPackage().getName();
        String ro="";
        String nu="";

        boolean oneofmine=false;
        if (packageName == "org.miracl.core.ED25519") {
            ro="edwards25519_XMD:SHA-256_ELL2_RO_TESTGEN";
            nu="edwards25519_XMD:SHA-256_ELL2_NU_TESTGEN";
            System.out.println("\nTesting HTP for curve ED25519");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.NIST256") {
            ro="P256_XMD:SHA-256_SSWU_RO_TESTGEN";
            nu="P256_XMD:SHA-256_SSWU_NU_TESTGEN";
            System.out.println("\nTesting HTP for curve NIST256");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.GOLDILOCKS") {
            ro="edwards448_XMD:SHA-512_ELL2_RO_TESTGEN";
            nu="edwards448_XMD:SHA-512_ELL2_NU_TESTGEN";
            System.out.println("\nTesting HTP for curve GOLDILOCKS");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.SECP256K1") {
            ro="secp256k1_XMD:SHA-256_SVDW_RO_TESTGEN";
            nu="secp256k1_XMD:SHA-256_SVDW_NU_TESTGEN";
            System.out.println("\nTesting HTP for curve SECP256K1");
            oneofmine=true;
        }
        if (packageName == "org.miracl.core.BLS12381") {
            ro="BLS12381G1_XMD:SHA-256_SVDW_RO_TESTGEN";
            nu="BLS12381G1_XMD:SHA-256_SVDW_NU_TESTGEN";
            System.out.println("\nTesting HTP for curve BLS12381");
            oneofmine=true;
        }

        if (!oneofmine) 
        {
            System.out.println("HTP not specified for this curve");
            return;
        }

        htp("",ro,nu);
        htp("abc",ro,nu);
        htp("abcdef0123456789",ro,nu);
        htp("a512_aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa",ro,nu);
    }
}
