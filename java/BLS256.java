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

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-00 */

// Minimal-signature-size variant

package org.miracl.core.XXX;

import org.miracl.core.RAND;
import org.miracl.core.HMAC;

public class BLS256 {
    public static final int BFS = CONFIG_BIG.MODBYTES;
    public static final int BGS = CONFIG_BIG.MODBYTES;
    public static final int BLS_OK = 0;
    public static final int BLS_FAIL = -1;

    public static FP16[] G2_TAB;

    static int ceil(int a,int b) {
        return (((a)-1)/(b)+1);
    }

/* output u \in F_p */
    static BIG hash_to_base(int hash,int hlen,byte[] DST,byte[] M,int ctr) {
        BIG q = new BIG(ROM.Modulus);
        int L = ceil(q.nbits()+CONFIG_CURVE.AESKEY*8,8);

        String tag= new String("H2C");
        byte[] TAG=tag.getBytes();
        byte[] INFO=new byte[TAG.length+1];
        for (int i=0;i<TAG.length;i++)
            INFO[i]=TAG[i];
        INFO[TAG.length]=(byte)ctr;

        byte[] PRK=HMAC.HKDF_Extract(hash,hlen,DST,M);
        byte[] OKM=HMAC.HKDF_Expand(hash,hlen,L,PRK,INFO);

        DBIG dx=DBIG.fromBytes(OKM);
        BIG u=dx.mod(q);
        return u;
    }

    /* hash a message to an ECP point, using SHA2, random oracle method */
    public static ECP bls_hash_to_point(byte[] M) {
        String dst= new String("BLS_SIG_ZZZG1-SHA512-SSWU-RO-_NUL_");
        BIG u=hash_to_base(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,dst.getBytes(),M,0);
        BIG u1=hash_to_base(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,dst.getBytes(),M,1);

        ECP P=ECP.hashit(u);
        ECP P1=ECP.hashit(u1);
        P.add(P1);
        P.cfp();
        P.affine();
        return P;
    }

    public static int init() {
        ECP8 G = ECP8.generator();
        if (G.is_infinity()) return BLS_FAIL;
        G2_TAB = PAIR256.precomp(G);
        return BLS_OK;
    }

    /* generate key pair, private key S, public key W */

    /* generate key pair, private key S, public key W */
    public static int KeyPairGenerate(byte[] IKM, byte[] S, byte[] W) {
        BIG r = new BIG(ROM.CURVE_Order);     
        int L = ceil(3*ceil(r.nbits(),8),2);
        ECP8 G = ECP8.generator();
        String salt=new String("BLS-SIG-KEYGEN-SALT-");
        String info=new String("");
        byte[] PRK=HMAC.HKDF_Extract(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,salt.getBytes(),IKM);
        byte[] OKM=HMAC.HKDF_Expand(HMAC.MC_SHA2,CONFIG_CURVE.HASH_TYPE,L,PRK,info.getBytes());

        DBIG dx=DBIG.fromBytes(OKM);
        BIG s=dx.mod(r);

        s.toBytes(S);
        G = PAIR256.G2mul(G, s);
        G.toBytes(W,true);
        return BLS_OK;
    }

    /* Sign message M using private key S to produce signature SIG */

    public static int core_sign(byte[] SIG, byte[] M, byte[] S) {
        ECP D = bls_hash_to_point(M);
        BIG s = BIG.fromBytes(S);
        D = PAIR256.G1mul(D, s);
        D.toBytes(SIG, true);
        return BLS_OK;
    }

    /* Verify signature given message m, the signature SIG, and the public key W */

    public static int core_verify(byte[] SIG, byte[] M, byte[] W) {
        ECP HM = bls_hash_to_point(M);

        ECP D = ECP.fromBytes(SIG);
        if (!PAIR256.G1member(D)) return BLS_FAIL;
        D.neg();
        
        ECP8 PK = ECP8.fromBytes(W);

// Use new multi-pairing mechanism
        FP48[] r = PAIR256.initmp();
        PAIR256.another_pc(r, G2_TAB, D);
        PAIR256.another(r, PK, HM);
        FP48 v = PAIR256.miller(r);

//.. or alternatively
//		ECP8 G=ECP8.generator();
//		if (G.is_infinity()) return BLS_FAIL;
//		FP48 v=PAIR256.ate2(G,D,PK,HM);

        v = PAIR256.fexp(v);
        if (v.isunity())
            return BLS_OK;
        return BLS_FAIL;
    }
}
